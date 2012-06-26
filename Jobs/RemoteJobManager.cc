#include "Jobs/RemoteJobManager.h"
#include "Jobs/JobManager.h"
#include "Core/Thread.h"
#include "Core/utils.h"
#include "Core/fdstream.h"
#include "IO/ConfigFileParser.h"
#include "Core/Color.h"
#include "Core/Scene.h"
#include "Core/Statistics.h"
#include "Core/Options.h"
#include <sys/wait.h>
#include <iostream>

class ProcessCommand {
 public:
  ProcessCommand( const StringVector &command ) : _command(command) {}
  ~ProcessCommand() {
    close(_p2);

    int status;
    waitpid( _pid, &status, WNOHANG );
  }
  void kill() {
    ::kill( _pid, SIGHUP );
    ::kill( _pid, SIGKILL );
  }
  
  bool executeCommand( int &localRead, int &localWrite ) {
    int filedes[2];
    int filedes1[2];
    
    // Re-attach file descriptors
    if ( pipe(filedes) == -1 ) {
      perror("Creating child pipe");
      return false;
    }
    if ( pipe(filedes1) == -1 ) {
      perror("Creating child pipe 2");
      return false;
    }
    
    // Fork and exec
    _pid = fork();

    if ( _pid == 0 ) {
      if ( dup2(filedes[0], 0) == -1 ) {
	perror("Setting child stdin");
	return false;
      }
      if ( dup2(filedes1[1], 1) == -1 ) {
	perror("Setting child stdout");
	return false;
      }
      close(filedes[1]); close(filedes1[0]);
      // Set up arguments
      char ** argv;
      argv = new char *[_command.size()+1];
      for ( unsigned i = 0; i < _command.size(); i++ ) {
	argv[i] = new char[_command[i].size()];
	strcpy( argv[i], _command[i].c_str() );
      }
      argv[_command.size()] = NULL;
      
      // And now exec...
      execvp( _command[0].c_str(), argv );
      perror("Exec failed");
      exit(0);
    }

    close(filedes[0]); close(filedes1[1]);
    localWrite = _p1 = filedes[1];
    localRead = _p2 = filedes1[0];
    
    return true;
  }
  
 private:
  StringVector _command;
  int _p1, _p2;
  pid_t _pid;
};


class RemoteThread : public Thread {
  friend std::ostream& operator<<(std::ostream &o, const RemoteThread *t ) {
    if ( t ) 
      o << t->_machine << ":" << t->_path;
    else
      o << "<Nil>";
    return o;
  }
 public:
  RemoteThread( std::string const &machine,
		std::string const &path,
		RemoteJobManager &manager ) : Thread("Remote Thread for " + machine),
					      _machine(machine),
					      _path(path),
					      _manager(manager),
					      _command(NULL) {
		  
		}

  ~RemoteThread() {
    std::cerr << "Closing " << this << std::endl;
    delete _command;
  }

  void earlyAbort() {
    std::cerr << "Aborting " << this << std::endl;
    close(_write);
    close(_read);
    _command->kill();
    delete _command;
    _command = NULL;
  }
  
  void readInput( std::istream &in, const Job &job ) {
    Color c;
    int x,y;
    int val[3];

    for ( x = job.startx; x < job.endx; x++ )
      for ( y = job.starty; y < job.endy; y++ ) {
	in >> val[0] >> val[1] >> val[2];
	c = Color( val[0] / 255.0f,
		   val[1] / 255.0f,
		   val[2] / 255.0f );
	_manager.putPixel( c, x, y );
      }
  }
  
  virtual void *run() {
    Job job;

    // First, start the remote process.
    if ( !startRemoteProcess() ) {
      ERROR("Can't start remote process for " << this);
      return NULL;
    }

    // Get acknowledgement ping first.
    char ackBuffer[ 4 ];
    ssize_t readResult = read( _read, ackBuffer, 4 );
    if ( readResult != 4 ) {
      std::cerr << "Got bad read result from slave " << this << std::endl;
      return NULL;
    }
    else {
      std::cerr << "Slave on " << this << " is online." << std::endl;
    }
    
    boost::fdistream in(_read);
    boost::fdostream out(_write);

    // Then, run forever.
    while (1) {

      // Get a job from the job manager.
      if ( _manager.getNextRenderingJob( job ) ) {
	// Write it to our command
	out << job << std::endl;
	
	// Then read the input
	readInput(in, job);
      }
      else {
	out << Job(0,0,0,0) << std::endl; // Ensure we flush the pipe.
	StatHolder s;
	if ( close(_write) ) {
	  perror("Couldn't close write pipe");
	}
	in >> s;
	std::cerr << this << " has stats " << s << std::endl;
	Statistics::AddStats(s);
	// If there are none left, we're done.
	return NULL;
      }
    }
  }

 protected:
  std::string _machine;
  std::string _path;
  RemoteJobManager &_manager;
  int _read, _write;
  ProcessCommand *_command;
  
  bool startRemoteProcess() {
    StringVector command;
    char buffer[40];
 
    command.push_back("ssh");
    command.push_back(_machine);
    command.push_back(_path);
    command.push_back("-quiet");
    command.push_back("-slave");
    command.push_back("-scene");
    command.push_back(Options::Instance()->scene());
    command.push_back("-sx");
    snprintf(buffer, 40, "%d", Options::Instance()->sizeX() ); command.push_back( buffer );
    command.push_back("-sy");
    snprintf(buffer, 40, "%d", Options::Instance()->sizeY() ); command.push_back( buffer );

    if ( Options::Instance()->samplesSet() ) {
      command.push_back( "-sample" );
      command.push_back( Options::Instance()->sampleType() );
      snprintf(buffer, 40, "%d", Options::Instance()->numSamples() ); command.push_back( buffer );
    }
    if ( Options::Instance()->filter() ) {
      command.push_back( "-filter" );
      command.push_back(Options::Instance()->filterType() );
    }
    
    _command = new ProcessCommand( command );

#ifdef DEBUGGING_SLAVES
    for ( unsigned int i = 0; i < command.size(); ++i ) {
      std::cerr << command[i] << " " << std::flush;
    }
    std::cerr << std::endl;
#endif
    return _command->executeCommand( _read, _write );
  }
};

void
RemoteJobManager::waitForFinish() {
  // Join all of the rendering threads
  for ( unsigned i = 0; i < _threadList.size(); i++ ) {
    _threadList[i]->join();
    delete _threadList[i];
    _threadList[i] = NULL;
  }
}

void 
RemoteJobManager::abort() {
  for ( unsigned i = 0; i < _threadList.size(); ++i ) {
    RemoteThread * r = dynamic_cast<RemoteThread *>(_threadList[i]);
    if ( r )
      r->earlyAbort();
  }
}

void
HandleBrokenPipe( int ) {
  std::cerr << "Ack! A slave died!" << std::endl;
}

void
RemoteJobManager::start() {

  // Resize the thread list
  _threadList.resize( _slaves.size() );

  // Set up a SIGPIPE handler, to handle the case where a slave goes
  // away
  signal( SIGPIPE, HandleBrokenPipe );
  
  // For each machine/path combo:
  for ( unsigned i = 0; i < _slaves.size(); i++ ) {
  
    // Create a remote thread.
    RemoteThread * thread = new RemoteThread( _slaves[i].machine, _slaves[i].path, *this );

    _threadList[i] = thread;
    
    // Start the remote thread in a joinable state.
    thread->start(false);
  }


}

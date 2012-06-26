#include "Core/Options.h"
#include "UI/UIInterfaceFactory.h"
#include "Core/Scene.h"
#include "Filter/FilterFactory.h"
#include "Filter/Filter2D.h"
#include <fstream>
#include <iostream>
#include <list>
#include <algorithm>
#include <string.h>
#include "unistd.h"
#include "config.h"

Options *
Options::_instance = NULL;

typedef std::list<std::string> StringList;

// Gets a numeric value, including suffixes 'K', 'M', 'G', 'T'
long getNumericValue( std::string str ) {
  char * endptr;
  const char * start = str.c_str();
  long value;

  value = strtol( start, &endptr, 0 );

  // No suffix? Just return
  if ( endptr == 0 )
    return value;

  // Bad result
  if ( endptr == start ) {
    std::cerr << "Bad numeric value: " << start << std::endl;
    return 0;
  }

  // Look for suffixes
  switch (endptr[0]) {
  case 'k':
  case 'K':
    value *= 1000;
    break;
  case 'm':
  case 'M':
    value *= 1000000L;
    break;
  case 'g':
  case 'G':
    value *= 1000000000L;
    break;
  case 't':
  case 'T':
    value *= 1000000000000L;
    break;
  default:
    std::cerr << "Ignoring unknown numeric modifier: " << endptr[0] << std::endl;
  }

  return value;
}


// Helper class for tokenization
class CIsComma : public std::unary_function<char, bool>
{
public:
  bool operator()(char c) const {
    return ( c == ',' );
  }
};
                                                                                
void Tokenize(StringList& roResult,
              std::string const& rostr)
{
  CIsComma roPred = CIsComma();
                                                                                
  //First clear the results list
  roResult.clear();
  std::string::const_iterator it = rostr.begin();
  std::string::const_iterator itTokenEnd = rostr.begin();
  while(it != rostr.end())
  {
    //Eat separators
    while(roPred(*it))
      it++;
    //Find next token
    itTokenEnd = std::find_if(it, rostr.end(), roPred);
    //Append token to result
    if(it < itTokenEnd)
      roResult.push_back(std::string(it, itTokenEnd));
    it = itTokenEnd;
  }
}


void
Options::Usage(std::string program) {

  std::cerr << "Usage: " << program << "           [-nthreads <num>]" << std::endl;
  std::cerr << "           [-slave] [-remote <machine1>,<machine2>,...]" << std::endl;
  std::cerr << "           [-remotelist <file> [<nremotes]] [-nframes <frames>]" << std::endl;
  std::cerr << "           [-scene <scene>] [-seed <seed>] [-quiet]" << std::endl;
  std::cerr << "           [-window <x0 y0 x1 y1>] [-sx <size x>] [-sy <size y>] [-s <size>]" << std::endl;
  std::cerr << "           [-sample <name> <number of samples>] [-filter <name>]" << std::endl;
  std::cerr << "           [-output <filename>] [-restart <filename>] [-list]" << std::endl;
  std::cerr << "           [-gi <params>]" << std::endl;
  std::cerr << "           [-ui <uitype>] [-h/--help] [scenefile]\n\n" << std::endl;

#ifdef USE_SWIG
  std::cerr << "If no scene is specified via the -scene argument, \n\
then a dynamic scene description is read from \n\
the given scene file. If the scene file is '-',\n\
then the scene is read from standard input.\n\n\
NOTE: Dynamic scene support is only available when\n\
properly configured at build time."
	    << std::endl;
#else
  std::cerr << "NOTE: Dynamic scene support is not supported in this build!" << std::endl;
#endif

  std::cerr << "\n Global Illumination parameters:" << std::endl;
  std::cerr << "-gi <params>   : Sets the global illumination parameters" << std::endl;
  std::cerr << "\tnp     - number of photons to use" << std::endl;
  std::cerr << "\tnep    - number of photons to use in estimate" << std::endl;
  std::cerr << "\test    - Size of estimate radius" << std::endl;
  std::cerr << "\tcnp    - number of caustic photons to use" << std::endl;
  std::cerr << "\tcnep   - number of photons to use in caustic estimate" << std::endl;
  std::cerr << "\tcest   - Size of caustic estimate radius" << std::endl;
  std::cerr << "\n Window parameters:" << std::endl;
  std::cerr << "-sx x          : Sets width of rendered image" << std::endl;
  std::cerr << "-sy y          : Sets height of rendered image" << std::endl;
  std::cerr << "-s  size       : Sets width and height of rendered image" << std::endl;
  std::cerr << "-window <x0 y0 x1 y1>" << std::endl;
  std::cerr << "               : Render the given window rectangle. Defaults to 0,0,sizeX,sizeY" << std::endl;

  std::cerr << "-nthreads <num>: Use <num> threads to render." << std::endl;
  std::cerr <<"                 Default on this machine is " << _instance->numThreads() << " threads." << std::endl;
  std::cerr << "-remote <machine1>,<machine2>,..." << std::endl;
  std::cerr << "               : Run on remote machines. Format is <machine>, or <machine:path>,"
	    << std::endl;
  std::cerr << "-remotelist <file> [<nremotes>]" << std::endl;
  std::cerr << "               : Read a list of remote machines from the given file." << std::endl;
  std::cerr << "                 If <nremotes> is present, use only that number of slaves." << std::endl;
  std::cerr << "-nframes <frames>" << std::endl;
  std::cerr << "               : For dynamic scenes, render the given number of frames." << std::endl;
  std::cerr << "-sample <name> <number of samples>" << std::endl;
  std::cerr << "               : Use the given sampler type with the given number of samples." << std::endl;
  std::cerr << "                 Available Types: " << std::flush;
  Scene::PrintSamplerTypes();
  std::cerr << "-filter <name> : Use the given filter type." << std::endl;
  std::cerr << "                 Available Types: " << std::flush;
  FilterFactory::PrintFilterTypes();
  std::cerr << "-scene <scene> : Render the given named scene." << std::endl;
  std::cerr << "-quiet         : Do not provide any output" << std::endl;
  std::cerr << "-output <filename>"<< std::endl;
  std::cerr << "               : Place output image in given file" << std::endl;
  std::cerr << "-restart <filename>" << std::endl;
  std::cerr << "               : Restart rendering of the given image. Note that " << std::endl;
  std::cerr << "                 identical rendering parameters must be given to this" << std::endl;
  std::cerr << "                 run as to that which generated the image" << std::endl;
  std::cerr << "-slave         : Run as a slave (not normally used from cmdline)" << std::endl;
  std::cerr << "-list          : List the available precompiled scenes." << std::endl;
  std::cerr << "\n UI parameters:" << std::endl;
  std::cerr << "-ui <uitype>   : Create the ui with the given type." << std::endl;
  std::cerr << "                 Available types: " << std::flush;
  UIInterfaceFactory::PrintUITypes();
  std::cerr << "                 Default is " << Instance()->uiType() << std::endl;
}


Options::Options() :
  _isSlave(false),
  _randomSeed(time(NULL)),
  _sizeX(512), _sizeY(512),
  _rectangle(0,0,0,0),
  _uiType("glut_threaded"),
  _numThreads(1),
  _outputFilename("raytracedscene"),
  _printList(false),
  _numSamples(-1),
  _filter(NULL),
  _interactive(false),
  _numFrames(1),
  _numPhotons(0), _photonsInEstimate(0), _estimateRadius(0.0),
  _numCausticPhotons(0), _photonsInCausticEstimate(0), _causticEstimateRadius(0.0)
  
{
#ifdef USE_SWIG
  // If we're using swig, then we are interactive by default.
  interactive(true);
#endif


  // Grab the number of CPUs, and set the number of threads to default
  // to this number.
  long nCPUs = sysconf(_SC_NPROCESSORS_CONF);
  _numThreads = nCPUs;
}
  
Options *
Options::Instance() {
  
  if ( _instance == NULL ) {
    std::cerr << "Must initialize options before using it!" << std::endl;
    exit(-1);
  }

  return _instance;
}


void
Options::Initialize( int argc, char ** argv ) {
  assert( _instance == NULL );

  std::string program = argv[0];
  
  _instance = new Options;

  for (int i = 1; i < argc; i++)
    {
      //////////////////// Num threads
      if ( !strcmp(argv[i], "-nthreads" ) ) {
	i++; assert( i < argc );
	int n;
	n = atoi(argv[i]);
	if ( n < 1 ) {
	  std::cerr << "Invalid number of threads specified: " << n << ", using 1" << std::endl;
	  n = 1;
	}
	_instance->numThreads(n);
      }

      //////////////////// Slaves
      else if ( !strcmp(argv[i], "-remote" ) ) {
	char * pwd = getenv("PWD");
	std::string rayPath = program;
	if ( rayPath[0] != '/' && pwd )
	  rayPath = std::string(pwd) + "/" + rayPath;
	i++; assert( i < argc );
	std::string remoteMachinesString = argv[i];
	StringList machineList;
	Tokenize( machineList, remoteMachinesString );
	Options::SlaveList slaveList;
	slaveList.resize( machineList.size() );
	StringList::iterator it; 
	unsigned j;
	for ( j = 0, it = machineList.begin();
	      j < machineList.size();
	      j++, it++ ) {
	  std::string::size_type pos = it->find(':');
	  if ( pos != std::string::npos ) {
	    slaveList[j].machine = it->substr(0, pos);
	    slaveList[j].path    = it->substr(pos+1);
	  }
	  else {
	    slaveList[j].machine = *it;
	    slaveList[j].path    = rayPath;
	  }
	}

	_instance->slaves(slaveList);
      }

      //////////////////// Remote list
      else if ( !strcmp(argv[i], "-remotelist" ) ) {
	char * pwd = getenv("PWD");
	std::string rayPath = program;
	if ( rayPath[0] != '/' && pwd )
	  rayPath = std::string(pwd) + "/" + rayPath;
	i++; assert( i < argc );
	std::ifstream inremote( argv[i] );
	if ( ! inremote.is_open() ) {
	  std::cerr << "Unable to open remote list file " << argv[i] << std::endl;
	  continue;
	}

	i++;
	int nremotes = -1;
	if ( i < argc ) {
	  int test = atoi(argv[i]);
	  if ( test > 0 )
	    nremotes = test;
	  else
	    --i;
	}
	
	Options::SlaveList slaveList;
	std::string input;
	while ( inremote >> input && nremotes != 0 ) {
	  std::string::size_type pos = input.find(':');
	  Slave slave;
	  if ( pos != std::string::npos ) {
	    slave.machine = input.substr(0, pos);
	    slave.path    = input.substr(pos+1);
	  }
	  else {
	    slave.machine = input;
	    slave.path    = rayPath;
	  }
	  slaveList.push_back(slave);
	  --nremotes;
	}

	_instance->slaves(slaveList);
      }
      
      //////////////////// Is Slave
      else if ( !strcmp(argv[i], "-slave") ) {
	_instance->isSlave(true);
	_instance->interactive(false);
      }
      
      //////////////////// Scene
      else if ( !strcmp(argv[i], "-scene") ) {
	++i; assert( i < argc );
	_instance->scene(argv[i]);
	_instance->interactive(false);
      }

      ///////////////////// Quiet
      else if ( !strcmp(argv[i], "-quiet") ) {
	_instance->quiet(true);
      }
      
      //////////////////// List
      else if ( !strcmp(argv[i], "-list") ) {
	_instance->printList(true);
      }
      
      //////////////////// Output
      else if ( !strcmp(argv[i], "-output") ) {
	++i; assert( i < argc );
	_instance->outputFilename(argv[i]);
      }

      //////////////////// Restart
      else if ( !strcmp(argv[i], "-restart") ) {
	++i; assert( i < argc );
	_instance->restartFilename(argv[i]);
      }
      
      //////////////////// Random Seed
      else if (!strcmp(argv[i], "-seed" ) ) {
	++i; assert( i < argc );
	_instance->randomSeed( strtoul( argv[i], NULL, 10 ) );
      }
      
      //////////////////// Rectangle
      else if ( !strcmp( argv[i], "-window" ) ) {
	unsigned int xs,xe,ys,ye;
	++i; assert( i < argc );
	xs = atol(argv[i]);
	++i; assert( i < argc );
	ys = atol(argv[i]);
	++i; assert( i < argc );
	xe = atol(argv[i]);
	++i; assert( i < argc );
	ye = atol(argv[i]);
	_instance->windowRectangle( Rectangle(xs,ys,xe,ye) );
      }

      
      // Size x
      else if (!strcmp(argv[i], "-sx"))
	{
	  unsigned int x;
	  i++;assert( i < argc );

	  x = strtoul( argv[i], NULL, 10 );
	  _instance->sizeX(x);
	}
      // Size y
      else if (!strcmp(argv[i], "-sy"))
	{
	  unsigned int y;
	  i++;assert( i < argc );

	  y = strtoul( argv[i], NULL, 10 );
	  _instance->sizeY(y);
	}

      // Size
      else if (!strcmp(argv[i], "-s"))
	{
	  unsigned int size;
	  i++;assert( i < argc );

	  size = strtoul( argv[i], NULL, 10 );
	  _instance->sizeX(size);
	  _instance->sizeY(size);
	}

      /////////////////// Number of frames
      else if (!strcmp(argv[i], "-nframes"))
	{
	  int nframes;
	  i++;assert( i < argc );

	  nframes = atoi( argv[i] );
	  _instance->numFrames( nframes );
	}
      
      //////////////////// UI Type
      else if ( !strcmp( argv[i], "-ui" ) ) {
	i++; assert( i < argc );
	_instance->uiType(argv[i]);
      }

      //////////////////// Sampling
      else if ( !strcmp(argv[i], "-sample" ) ) {
	i++;assert( i < argc );
	std::string sampleType = argv[i];
	i++;assert( i < argc );
	int numSamples = atoi(argv[i]);
	if ( numSamples < 1 ) {
	  std::cerr << "Invalid number of samples specified: " << numSamples << ", using 1" << std::endl;
	  numSamples = 1;
	}
	_instance->_numSamples = numSamples;
	_instance->_sampleType = sampleType;
      }
      //////////////////// Filtering
      else if ( !strcmp(argv[i], "-filter" ) ) {
	++i; assert( i < argc );
	Filter2D *f = FilterFactory::findFilter2D( argv[i] );
	if ( !f )
	  std::cerr << "Unable to find filter " << argv[i] << std::endl;
	else {
	  _instance->_filter = f;
	  _instance->_filterType = argv[i];
	}
      }

      //////////////////// Global illumination
      else if ( !strcmp( argv[i], "-gi" ) ) {
	i++; assert( i < argc );
	std::string giOptions = argv[i];
	StringList giList;
	Tokenize( giList, giOptions );
	StringList::iterator it;
	for ( it = giList.begin(); it != giList.end(); ++it ) {
	  std::string::size_type pos = it->find('=');
	  if ( pos != std::string::npos ) {
	    std::string param = it->substr(0, pos);
	    std::string value = it->substr(pos+1);

	    // DEBUG
	     std::cerr << "param/value = " << param << " " << value << std::endl;
	    // DEBUG

	    if ( param == "np" )
	      _instance->numPhotons( getNumericValue(value) );
	    else if ( param == "nep" )
	      _instance->photonsInEstimate( getNumericValue(value) );
	    else if ( param == "est" )
	      _instance->estimateRadius( atof(value.c_str()) );
	    else if ( param == "cnp" )
	      _instance->numCausticPhotons( getNumericValue(value) );
	    else if ( param == "cnep" )
	      _instance->photonsInCausticEstimate( getNumericValue(value) );
	    else if ( param == "cest" )
	      _instance->causticEstimateRadius( atof(value.c_str()) );
	    else
	      std::cerr << "unknown global illumination parameter " << param << std::endl;
	  }
	  else {
	    std::cerr << "Bad global illumination option " << *it << std::endl;
	  }
	} // Each parameter

	// Set up defaults if they're not yet set.
	if ( _instance->photonsInEstimate() == 0 )
	  _instance->photonsInEstimate( _instance->numPhotons() / 10 );
	if ( _instance->photonsInCausticEstimate() == 0 )
	  _instance->photonsInCausticEstimate( _instance->numCausticPhotons() / 10 );
      }
      //////////////////// Global illumination
      
	else if ( !strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
	Usage(program);
	exit(0);
      }
      // Unknown argument!
#ifdef USE_SWIG
      else {
	_instance->sceneFile( argv[i] );
	_instance->interactive(false);
      }
#else
      else
	{
	  std::cerr << "Unknown argument '" << argv[i] << "'" << std::endl;
	}
#endif 
    }
  
  const Rectangle &rectangle = _instance->windowRectangle();

  // If the rectangle isn't set, then set it to 0,0,sizeX,sizeY.
  if ( !rectangle.set() ) {
    _instance->windowRectangle( Rectangle(0, 0, _instance->sizeX(), _instance->sizeY()) );
  }
  else {
    // Ensure size is at least as big as rectangle.
    if ( _instance->sizeX() < (rectangle._endX - rectangle._startX) )
      _instance->sizeX( (rectangle._endX - rectangle._startX) );
    if ( _instance->sizeY() < (rectangle._endY - rectangle._startY) )
      _instance->sizeY( (rectangle._endY - rectangle._startY) );
  }

  // Slaves use only the nil UI
  if ( _instance->isSlave() )
    _instance->uiType("nil");

  // If we have slaves and we're using glut, upgrade it to
  // glut_threaded.
  if ( _instance->slaves().size() > 0 && _instance->uiType() == "glut" )
    _instance->uiType( "glut_threaded" );

}

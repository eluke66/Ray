#include <iostream>
#include <iomanip>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "UI/UIInterfaceFactory.h"
#include "UI/UserInterface.h"
#include "Jobs/JobManager.h"
#include "Core/Raytracer.h"
#include "Core/Scene.h"
#include "Core/Image.h"
#include "Core/Statistics.h"
#include "Core/Options.h"
#include "Core/InstantRadiosity.h"
#include "config.h"
#include "Core/utils.h"
#include <execinfo.h>
#include <cxxabi.h>

static SceneMakerMap *AllScenes = NULL;
static UI *DumpUI = NULL;

void dumpTrace( int sig ) {
  
  static bool IN_DUMP = false;

  signal( sig, NULL );
  if ( IN_DUMP ) {
    printf("How'd we get here?!\n");
    exit(0);
  }
  IN_DUMP = true;
  
  if ( !Options::Instance()->quiet() ) {
    std::cerr << "Caught signal " << sig << std::endl;
  }
  // Print Backtrace
  void *array[10];
  size_t size;
  char **strings;
  size_t i;
  
  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);
  
  printf ("Obtained %zd stack frames.\n", size);
  
  for (i = 0; i < size; i++) {

    //printf("\tSTRINGS: %s\n", strings[i]);
    int stat;
    char buffer[1024];
    memset(buffer,0,1024);
    char * startSpot = strchr(strings[i],'(');
    //printf("\tStart: %s\n", startSpot);
    char * demangled = 0;
    if ( startSpot ) {
      char * endSpot = strchr(strings[i],'+');
      if ( endSpot ) {
	int numChars = endSpot - startSpot-1;
	strncpy(buffer, startSpot+1, numChars );
	demangled = abi::__cxa_demangle(buffer,0,0,&stat);
      }
    }

    if ( demangled )
      printf("%s\n", demangled);
    else
      printf("%s (%d)\n", strings[i], stat );
	     
  }
  
  // Whatever, die now...free (strings);
  exit(0);
}


void dumpImage( int sig ) {
  if ( !Options::Instance()->quiet() ) {
    std::cerr << "Caught signal " << sig << std::endl;
  }
  if ( !Options::Instance()->isSlave() && DumpUI ) {
    DumpUI->dump();
  }

  exit(0);
}

SceneMakerMap *GetAllScenes() {
  if ( AllScenes == NULL )
    AllScenes = new SceneMakerMap;
  return AllScenes;
}

using std::cerr;
using std::setprecision;
using std::string;


bool g_quietMode = true;

void CleanUp() {
  Statistics::EndRendering();
  if ( !Options::Instance()->quiet() )
    Statistics::Report();
}
// END OF GLOBALS


static RaytracerList tracerList;
void InstallScene( Scene *scene );
UI * SetupUI( int argc, char ** argv, Scene *scene );
void RenderScene(UI *theUI, Scene *scene);

#ifdef USE_SWIG
typedef Scene *(*SceneGetter)(FILE *, const char *);

#include <dlfcn.h>
#include PYTHON_VERSION

SceneGetter mainSceneGetter = NULL;
static Scene * loadDynamicScene( Options * options ) {
  SceneGetter sceneGetter = NULL;
  Scene *scene = NULL;
  
  if ( options->sceneFile() != "" ) {
    setenv("PYTHONPATH", getenv("PWD"), 0 );
    Py_Initialize();
    void *handle = dlopen("_Core.so", RTLD_LAZY|RTLD_GLOBAL);
    if ( !handle ) {
      std::cerr << "Unable to open Core python library: "
		<< dlerror() << std::endl;
      return NULL;
    }
    sceneGetter = mainSceneGetter;
    if  (!sceneGetter) {
      std::cerr << "Unable to find scene getter function: "
		<< dlerror() << std::endl;
      return NULL;
    }

    
    FILE * sceneFile = NULL;
    if ( options->sceneFile() == "-" ) // stdin
      sceneFile = stdin;
    else
      sceneFile = fopen(options->sceneFile().c_str(), "r" );
    if ( !sceneFile ) {
      std::string err("Unable to open scene file ");
      err += options->sceneFile();
      perror(err.c_str());
      return NULL;
    }
    
    scene = (*sceneGetter)(sceneFile, options->sceneFile().c_str());
    dlclose(handle);
    if ( scene ) {
      std::cerr << "WOOT!" << std::endl;
    }
    //  Py_Finalize();
  } // We have a dynamic scene
  return scene;
}

SceneGetter interactiveSceneGetter = NULL;
Scene * interactiveScene( Options *options ) {
  Scene *scene = NULL;
  static bool initialized = false;

  if ( !initialized ) {
    setenv("PYTHONPATH", getenv("PWD"), 0 );
    Py_Initialize();
    void *handle = dlopen("_Core.so", RTLD_LAZY|RTLD_GLOBAL);
    if ( !handle ) {
      std::cerr << "Unable to open Core python library: "
		<< dlerror() << std::endl;
      return NULL;
    }
    initialized = true;
  }
  assert(interactiveSceneGetter);
  scene = (*interactiveSceneGetter)(NULL, NULL);

  if ( scene ) 
    std::cerr << "ok!" << std::endl;
  else
    std::cerr << "missing or invalid scene" << std::endl;
  return scene;

}

#else
static Scene * loadDynamicScene( Options * options ) {return NULL;}
#endif

Scene * getScene( bool interactive ) {
  Scene *scene = NULL;
  Options * options = Options::Instance();


  if ( interactive ) {
    scene = interactiveScene(options);
  }
  else 
    scene = loadDynamicScene( options );
  if ( scene == NULL ) {
    std::string renderScene;
    if ( !options->scene().empty() )
      renderScene = options->scene();
    else {
      std::cerr << "No scene specified!" << std::endl;
      exit(0);
    }
    if ( AllScenes->find(renderScene) == AllScenes->end() ) {
      std::cerr << "Unknown scene " << renderScene << std::endl;
      renderScene = AllScenes->begin()->first;
      options->scene(renderScene);
    }
    if ( !options->quiet() )
      std::cerr << "Rendering scene " << renderScene << std::endl;
    scene = (*AllScenes)[renderScene]();
  }
  
  if ( options->samplesSet() ) {
    scene->setSampling( Scene::LookupSampler(options->sampleType()),  options->numSamples());
  }
  
  scene->setImage(new Image(options->sizeX(), options->sizeY()) );
  
  if ( getenv("RADIOSITY") ) {
    InstantRadiosity ir(scene);
    ir.addRadiosityLights();
  }

  scene->preprocess();
  if ( options->filter() ) {
    scene->setFilter( options->filter() );
  }

  return scene;

}

int main( int argc, char **argv ) {
  Statistics::Initialize();
  Options::Initialize(argc, argv);
  Options *options = Options::Instance();

  srand48(options->randomSeed());
  srand(options->randomSeed());
  Scene * scene = NULL;
  UI *theUI = SetupUI (argc, argv, NULL);
  
  bool interactive = options->interactive(); 
  
  
  if ( interactive ) {
    bool done = false;
    while (!done) {
      Statistics::StartPreProcessing();
      scene = getScene(true);
      InstallScene(scene);
      Statistics::EndPreProcessing();
      RenderScene(theUI, scene);
      done = true;
    }
    theUI->finish();
  }
  else {
    scene = getScene(false);
    InstallScene(scene);
    Statistics::EndPreProcessing();
    RenderScene( theUI, scene );
    std::cerr << "Finishing!"<< std::endl;
    theUI->finish();

  }
  
}

int main1(int argc, char** argv)
{
  Statistics::Initialize();
  Options::Initialize(argc, argv);
  Options *options = Options::Instance();
  Scene * scene = NULL;

  if ( options->printList() ) {
    if ( !options->quiet() )
      std::cerr << "We have " << AllScenes->size() << " scenes available!" << std::endl;
    for ( SceneMakerMap::iterator it = AllScenes->begin(); it != AllScenes->end(); ++it )
      std::cerr << it->first << std::endl;
    exit(0);
  }
  
  scene = loadDynamicScene( options );
  
  if ( scene == NULL ) {
    std::string renderScene = AllScenes->begin()->first;
    if ( !options->scene().empty() )
      renderScene = options->scene();
    else
      options->scene( renderScene );
    if ( AllScenes->find(renderScene) == AllScenes->end() ) {
      std::cerr << "Unknown scene " << renderScene << std::endl;
      renderScene = AllScenes->begin()->first;
      options->scene(renderScene);
    }
    if ( !options->quiet() )
      std::cerr << "Rendering scene " << renderScene << std::endl;
    scene = (*AllScenes)[renderScene]();
  }
  
  if ( options->samplesSet() ) {
    scene->setSampling( Scene::LookupSampler(options->sampleType()),  options->numSamples());
  }
  
  scene->setImage(new Image(options->sizeX(), options->sizeY()) );

  if ( getenv("RADIOSITY") ) {
    InstantRadiosity ir(scene);
    ir.addRadiosityLights();
  }

  scene->preprocess();
  if ( options->filter() ) {
    scene->setFilter( options->filter() );
  }
  
  UI * ui = SetupUI( argc, argv,
		     scene );
  Statistics::EndPreProcessing();

  RenderScene( ui, scene );
  return 0;
}



UI *
SetupUI(int argc, char ** argv, Scene *scene ){

  Options * options = Options::Instance();
  int xres = options->sizeX();
  int yres = options->sizeY();
  std::string uiType = options->uiType();
  
  // Create the UI.
  UI * theUI = UIInterfaceFactory::CreateUserInterface( uiType, xres, yres );
  assert( theUI );

  if ( options->numThreads() == 1 ) {
    // For right now, just use a single ray tracer.
    Raytracer * tracer = new Raytracer ( theUI, scene );
    tracer->setFullWindowSize( xres,
			       yres );
    const Options::Rectangle & rect = options->windowRectangle();
    tracer->setWindowRectangle( rect._startX, rect._startY,
				rect._endX, rect._endY );
    tracerList.push_back(tracer);
  }
  else {
    tracerList.resize( options->numThreads() );
    for ( unsigned int i = 0; i < options->numThreads(); i++ ) {
      Raytracer * tracer = new Raytracer ( theUI, scene );
      tracer->setFullWindowSize( xres,
				 yres );
      tracerList[i] = tracer;
    }
  }
  
  // Initialize UI
  theUI->initialize( argc, argv );

  // Set up signals
  signal( SIGHUP, dumpImage );
  signal( SIGTERM, dumpImage );
  signal( SIGINT, dumpImage );
  signal( SIGSEGV, dumpTrace );
  signal( SIGABRT, dumpTrace );
  DumpUI = theUI;

  // Set the cleanup function, called after rendering is done.
  theUI->setCleanupFunction( CleanUp );
  
  return theUI;
}
  
void RenderScene(UI *theUI, Scene *scene) {
  ThreadedUI * ui = dynamic_cast<ThreadedUI *>(theUI);
  if ( ui ) {
    JobManager *jm = ui->getJobManager();
    assert(jm);

    // If we only have one frame specified, then don't add any frame
    // manipulators
    if ( Options::Instance()->numFrames() > 1 ) {
      jm->addFrameFuncs( scene->getFrameManipulators() );
    }
    else {
      if ( scene->getFrameManipulators().size() > 0 ) {
	WARN("Ignoring frame manipulators when only rendering a single frame!");
      }
    }
  }
    
  theUI->start( tracerList );
}

void InstallScene( Scene *scene ) {
  RaytracerList::iterator it;

  for ( it = tracerList.begin();
	it != tracerList.end();
	++it ) {
    (*it)->scene(scene);
  }

}

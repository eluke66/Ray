#include "UI/NilUI.h"
#include "Core/Scene.h"
#include "Jobs/JobManager.h"
#include "UI/UIInterfaceFactory.h"
#include "Jobs/SlaveJobManager.h"
#include "Jobs/RemoteJobManager.h"
#include "Core/Raytracer.h"
#include <fstream>
#include "UI/UIFunc.h"

NilUI::NilUI( int width, int height ) : UI(width,height) {


}

NilUI::~NilUI() {


}


void 
NilUI::initialize( int argc, char ** argv ) {
  // No-op
}

void * 
NilUI::start( RaytracerList &raytracers ) {

  // If we're a slave, use a slave job manager
  if ( Options::Instance()->isSlave() ) {
    _jobManager = new SlaveJobManager;

    // Start the job manager
    startJobManager( raytracers, _width, _height );
    return NULL;
  }
  
  const Options::SlaveList &slaves = Options::Instance()->slaves();
  
  // If we have only a single raytracer, don't bother using the job
  // manager. 
  if ( raytracers.size() == 1 && slaves.size() == 0 ) {
    raytracers.front()->trace();
    _jobManager = NULL;
  }
  else {
    // Create Job Manager
    _jobManager = new JobManager();

    _jobManager->extraTracers( slaves.size() );
    _jobManager->addFrameFunc( new UIFunc( this ) );
    if ( !Options::Instance()->restartFilename().empty() )
      _jobManager->restartImage( _image );
    
     // Start the job manager
    startJobManager( raytracers, _width, _height );
  }

  // Set up any remote jobs if needed
  if ( slaves.size() > 0 ) {
    _remoteJobManager =
      new RemoteJobManager( slaves, *this, *_jobManager );
    _remoteJobManager->start();
  }
  
  
  return NULL;
}

// Register us with the UI
struct NilUIInit { NilUIInit() { UIInterfaceFactory::AddUICreator( new UICreatorImpl<NilUI>(), "nil" ); } }; static NilUIInit _theNilUIInit;


void 
NilUI::finish() {

  
  if ( _jobManager ) {
    // Wait for job manager to finish
    _jobManager->waitForFinish();
  }

  if ( _remoteJobManager ) {
    _remoteJobManager->waitForFinish();
  }

  // Finish rest of UI tasks
  UI::finish();
}


#include <iostream>
#include <fstream>
#include "Jobs/SlaveJobManager.h"
#include "Core/Thread.h"
#include "Core/Statistics.h"

SlaveJobManager::SlaveJobManager() {
}

SlaveJobManager::~SlaveJobManager() {

}

void
SlaveJobManager::start() {

  std::cout << "ACK" << std::endl;
  // Setup and run raytracers;
  runRaytracers();
}


bool
SlaveJobManager::getNextRenderingJob( Job &job ) {

  if ( std::cin.eof() ) {
    // Closed. We're done.
    _done = true;
    _jobQueueWaiter->signal(); // We're done with this queue.
    return false;
  }
  
    
  // Read jobs from stdin. When stdin is closed, we're done.
  std::cin >> job;
  
  if ( std::cin.eof() ) {
    std::cout << Statistics::StatsObject << std::endl;
  }
  // Job is valid.
  return true;
}


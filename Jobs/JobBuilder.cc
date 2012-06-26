#include "Jobs/JobBuilder.h"
#include "Core/Image.h"
#include "Core/utils.h"
#include "Core/Color.h"

#include <math.h>
#include <list>
#include <vector>

void
JobBuilder::MakeJobsStripes(JobQueue &jobQueue, unsigned width, unsigned height,
			    unsigned int numJobs, int startx, int starty ) {
  int jobSize = width / numJobs;
  int xStart = startx;
  int xEnd   = startx + jobSize;

  
  for ( unsigned i = 0; i < numJobs; i++ ) {
    Job j( xStart, starty, xEnd, starty+height-1 );
    jobQueue.push(j);
    xStart = xEnd;
    xEnd = xStart + jobSize;
  }
  // Finally, if the end isn't at width-1, add a final job
  if ( (unsigned)xStart < startx + width-1 )
    jobQueue.push( Job( xStart, starty, startx + width-1, starty + height-1) );

}

void
JobBuilder::MakeJobsTiles(JobQueue &jobQueue, unsigned width, unsigned height,
			  unsigned int numJobs, int startx, int starty) {
  
  int sqrtJobs = (int)sqrtf((float)numJobs);

  // Size of normal tiles
  int xSize = width / sqrtJobs;
  int ySize = height / sqrtJobs;

  // Size of final tiles on a row/column
  int xResidual = width - (sqrtJobs-1)*xSize; 
  int yResidual = height - (sqrtJobs-1)*ySize; 
  
  int xStart = startx;
  int yStart = starty;

  // Load all of the easy jobs
  for ( int x = 0; x < sqrtJobs-1; ++x ) {
    for ( int y = 0; y < sqrtJobs-1; ++y ) {
      jobQueue.push( Job( xStart,
			  yStart,
			  xStart + xSize,
			  yStart + ySize ) );
      yStart += ySize;
    }
    
    // Last job on a column might be bigger 
    jobQueue.push( Job( xStart,
			yStart,
			xStart + xSize,
			yStart + yResidual ) );
    yStart = starty;
    xStart += xSize;
  }

  // Last jobs on each row might be bigger
  for ( int y = 0; y < sqrtJobs-1; ++y ) {
    jobQueue.push( Job( xStart,
			yStart,
			xStart + xResidual,
			yStart + ySize ) );
    yStart += ySize;
  }
  
  // Final job might be the biggest of all.
  jobQueue.push( Job( xStart,
		      yStart,
		      xStart + xResidual,
		      yStart + yResidual ) );
  
}

struct Stripe {
  Stripe() : _start(-1), _finish(-1),_colspan(1) {}
  Stripe(int s,int e):_start(s), _finish(e), _colspan(1) {}
  bool valid() const { return _start != -1; }
  bool finished() const { return _finish != -1; }
  int start() const { return _start; }
  void start(int s) {_start = s; }
  int finish() const { return _finish; }
  void finish(int s) {_finish = s; }
  int colspan() { return _colspan; }
  void incColspan() { ++_colspan; }
private:
  int _start,_finish,_colspan;
};

bool IsBlack( const Color &c ) {
  return ABSCLOSE(c.r(),0,1e-3) &&
    ABSCLOSE(c.g(),0,1e-3) &&
    ABSCLOSE(c.b(),0,1e-3);
}

typedef std::vector<Stripe> StripeList;
typedef std::vector< StripeList > StripeColList;

struct Candidate {
public:
  Candidate( int columnStart, const Stripe &stripe );

  bool hasEqualSpan( const Stripe &stripe ) const;
  void addSpan( const Stripe &stripe );
  void okay(bool okay);
  bool okay() const;
  Job  toJob(int height) const;
private:
  Job _job;
  bool _okay;
};

typedef std::vector<Candidate> CandidateList;

void
JobBuilder::MakeJobsFromImage(JobQueue &jobQueue, Image *image, unsigned int numJobs ) {

  assert(image);
  int width = image->width();
  int height = image->height();
  unsigned int totalStripes = 0;
  
  // Each black pixel should be part of a job.

  
  StripeColList stripes;
  stripes.resize( width );

  // Stripes: Scan, top to bottom, for column stripes.
  for ( int y = 0; y < height-1; ++y ) {
    for ( int x = 0; x < width; ++x ) {

      if ( IsBlack( image->getPixel(x,y) ) ) {
	
	// If we have no current stripe, create a new one.
	if ( stripes[x].empty() ||
	     !stripes[x].back().valid() ) {
	  Stripe s;
	  s.start(y);
	  stripes[x].push_back(s);
	  ++totalStripes;
	}
      }
      else {
	if ( !stripes[x].empty() &&
	     stripes[x].back().valid() &&
	     !stripes[x].back().finished() ) {
	  // If the current stripe is valid, then cap it here.
	  stripes[x].back().finish(y);
	}
      }
      
    }
  }

  if ( totalStripes > numJobs ) {
    // Optimization: If adjacent columns have equal y spans, combine
    // them.

    // ALGORITHM:
    unsigned totalPixels = 0;
    CandidateList candidates;
    
    // For each column:
    for ( int x = 0; x < width; ++x ) {

      // For each stripe in the column:
      for ( StripeList::iterator it = stripes[x].begin();
	    it != stripes[x].end();
	    ++it ) {
      
	// If the previous column has a job with the same y span, then
	// increment the colspan of that job. Make it 'ok'.
	CandidateList::iterator canIt;
	for ( canIt = candidates.begin(); canIt != candidates.end(); ++canIt ) {
	  if ( canIt->hasEqualSpan( *it ) ) {
	    canIt->addSpan( *it );
	    canIt->okay(true);
	    break;
	  }
	}
	
	// If there are no matching candidates, then create a new job for
	// the stripe, and place it in the 'candidates' list. Make it
	// 'ok'.
	if ( canIt == candidates.end() ) {
	  candidates.push_back( Candidate( x, *it ) );
	}
      }
      
      // At the end of the column, take everything from the candidate
      // list that is not okay, and add it to the job queue. Mark all
      // candidates as not okay.
      CandidateList newCandidates;
      for ( CandidateList::iterator canIt = candidates.begin();
	    canIt != candidates.end();
	    ++canIt ) {
	if ( !canIt->okay() ) {
	  Job j =  canIt->toJob(height);
	  jobQueue.push(j );
	  totalPixels += (j.endx-j.startx)*(j.endy-j.starty);
	}
	else {
	  canIt->okay(false);
	  newCandidates.push_back(*canIt);
	}
      }
      candidates = newCandidates;
    }

    // Place everything left in the candidate list into the job queue
    for ( CandidateList::iterator canIt = candidates.begin();
	  canIt != candidates.end();
	  ++canIt ) {
      Job j =  canIt->toJob(height);
      totalPixels += (j.endx-j.startx)*(j.endy-j.starty);
      jobQueue.push( j );
    }
    

    if ( jobQueue.size() < numJobs ) {
      // Further optimization: If we have fewer jobs now than required,
      // figure out how big the average job will need to be, and carve
      // up the jobs appropriately.
      
      // Split jobs of total size N into T pieces. Each new job will be
      // of size N/T each.
      int jobSize = totalPixels / numJobs;
      
      JobQueue finalQueue;
      while ( ! jobQueue.empty() ) {
	Job &j = jobQueue.front(); 
	int jobPixels = (j.endx-j.startx)*(j.endy-j.starty);
	int numSplits = jobPixels / jobSize;
	if ( numSplits == 0 ) numSplits = 1;
	JobQueue newQueue;
	MakeJobsTiles( newQueue, (j.endx-j.startx), (j.endy-j.starty)+1, numSplits, j.startx, j.starty );
	while ( !newQueue.empty() ) { finalQueue.push( newQueue.front() ); newQueue.pop(); }
	jobQueue.pop();
      }
      while ( !finalQueue.empty() ) { jobQueue.push( finalQueue.front() ); finalQueue.pop(); }
    }
    
  }
  else { 
    // Fashion stripes into jobs.
    for ( int x = 0; x < width; ++x ) {
      for ( StripeList::iterator it = stripes[x].begin();
	    it != stripes[x].end();
	    ++it ) {
	int end = it->finish();
	if ( end == -1 )
	  end = height-1;
	jobQueue.push( Job( x,
			    it->start(),
			    x+1,
			    end ) );
      }
    }
  }
}


Candidate::Candidate( int columnStart, const Stripe &stripe ) :
  _job( columnStart, stripe.start(), columnStart+1, stripe.finish() ),
  _okay(true)
{
}

bool 
Candidate::hasEqualSpan( const Stripe &stripe ) const {
  return stripe.start() == _job.starty &&
    stripe.finish() == _job.endy;
}


void 
Candidate::addSpan( const Stripe &stripe ) {
  assert( hasEqualSpan(stripe) );

  _job.endx++;
}


void 
Candidate::okay(bool okay) {
  _okay = okay;
}


bool 
Candidate::okay() const {
  return _okay;
}

Job  
Candidate::toJob(int height) const {
  Job tmp(_job);

  if ( tmp.endy == -1 )
    tmp.endy = height-1;
  return tmp;
}

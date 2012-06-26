#ifndef JOB_H_
#define JOB_H_

#include <queue>
#include <iostream>

struct Job {
  Job() {}
  Job( int sx, int sy, int ex, int ey ) :
    startx(sx), starty(sy), endx(ex), endy(ey) {}
  int startx, starty, endx, endy;
};


extern std::ostream& operator<<( std::ostream &in, const Job& job );
extern std::istream & operator>>(std::istream &s, Job &job);

typedef std::queue<Job> JobQueue;
#endif

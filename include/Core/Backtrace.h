#ifndef _BACKTRACE_H_
#define _BACKTRACE_H_

// Returns a new-ed array of char arrays, and copies
// the number of frames returned into numFrames.
extern char ** getBacktrace(int &numFrames);

#endif

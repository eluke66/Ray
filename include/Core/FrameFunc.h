#ifndef _Frame_Func_h_
#define _Frame_Func_h_

class Scene;

class FrameFunc {
public:
  FrameFunc() : _nFrames(0) {}
  virtual ~FrameFunc() {}
  virtual void setNumFrames(int nFrames) { _nFrames = nFrames; }
  virtual void operator()(int whichFrame) = 0;
  virtual void doRegister(Scene * scene) = 0;
protected:
  int _nFrames;
};

#endif

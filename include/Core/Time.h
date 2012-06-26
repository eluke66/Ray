
#ifndef Time_h
#define Time_h

class Time {
 public:
  static double currentSeconds();
 private:
  Time();
  Time(const Time&);
  Time& operator=(const Time&);

  static void initialize();
};

class Timer {
public:
  Timer() : _start( Time::currentSeconds() ) {}
  double time() const
  { return Time::currentSeconds() - _start; }

  void restart() { _start = Time::currentSeconds(); }
private:
  double _start;
};
#endif

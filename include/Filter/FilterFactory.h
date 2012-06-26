
#ifndef FilterFactory_h
#define FilterFactory_h

#include <string>

class Filter1D;
class Filter2D;

class FilterFactory {
 public:
  static Filter1D* findFilter1D(const std::string& filterspec);
  static Filter2D* findFilter2D(const std::string& filterspec);
  static void PrintFilterTypes();
 private:
  FilterFactory(const FilterFactory&);
  ~FilterFactory();
};

#endif

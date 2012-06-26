#ifndef _ProjectionMap_h_
#define _ProjectionMap_h_

class Object;

class ProjectionMap {
public:
  ProjectionMap();
  virtual ~ProjectionMap();

  virtual void build( Object * object, double resolution ) = 0;
  virtual void nextCell( int &cookie, Vector &output ) = 0;
  
protected:
};

#endif

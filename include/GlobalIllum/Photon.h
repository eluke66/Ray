#ifndef PHOTON_H_
#define PHOTON_H_

#include "Core/Color.h"
#include "Core/Point.h"

class Photon {
public:
  Photon() {}
  Photon(const Point &pos,
	 const Color &color,
	 short plane,
	 unsigned char theta,
	 unsigned char phi) :
    _pos(pos),
    _color(color),
    _plane(plane),
    _theta(theta),
    _phi(phi) {}

  const Point &pos() const { return _pos; }
  void pos(const Point &point ) { _pos = point; }
  const Color &color() const { return _color; }
  void color(const Color &c ) { _color = c; }
  short plane() const { return _plane; }
  void plane(short value) { _plane = value; }
  unsigned char theta() const { return _theta; }
  unsigned char phi() const { return _phi; }
  void dir( unsigned char t, unsigned char p ) {
    _theta = t; _phi = p;
  }
  
private:
  Point _pos;
  Color _color;
  short _plane;
  unsigned char _theta, _phi;
};

#endif

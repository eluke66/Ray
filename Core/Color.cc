
#include "Core/Color.h"
#include <iostream>

std::ostream& operator<<(std::ostream& out, const Color& c)
{
  out << '[' << c.r() << ", " << c.g() << ", " << c.b() << ']';
  return out;
}

Color
Color::Lerp( const Color &c1, const Color &c2, float frac )
{
  assert( frac <= 1.0 && frac >= 0.0 );
  
  return c1*frac + c2*(1-frac);

}
  

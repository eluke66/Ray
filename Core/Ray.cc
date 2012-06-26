
#include "Core/Point.h"
#include "Core/Vector.h"
#include "Core/Ray.h"
#include <iostream>

std::ostream& operator<<(std::ostream& out, const Ray& r)
{
  out << r.origin() << " -> "
      << r.direction();
  return out;
}

/**
 * Header file for shapes, rays, and such.
 */

#ifndef GEOMETRYH
#define GEOMETRYH

#include "algebra3.h"

class ray;

class ray
{
 public:
  ray();
  ray(const vec3 p, const vec3 d);
 
  vec3 pos;
  vec3 dir;
}

#endif

/**
 * Header file for our ray tracer.
 */

#ifndef GEOMETRYH
#define GEOMETRYH

#include "algebra3.h"

class ray;
class shape;
class scene;

class ray
{
public:
  ray();
  ray(const vec3 p, const vec3 d);
 
  vec3 pos;
  vec3 dir;
}

class shape
{
public:
  virtual bool intersect(ray r);
}

class scene
{
public:
  void readScene(string path);

  // variables to hold things in the scene
}
#endif

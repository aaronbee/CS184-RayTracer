/**
 * Header file for our ray tracer.
 */

#ifndef RAYTRACERH
#define RAYTRACERH

#include "algebra3.h"
#include <string.h>

class ray;
class shape;
class scene;
class camera;

class ray
{
public:
  ray(const vec3 p, const vec3 d) : pos(p), dir(d) { }
  vec3 getPos() { return pos; }
  vec3 getDir() { return dir; }

private:
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
  int getWidth() { return width; }
  int getHeight() { return height; }
  int getFov() { return fov; }
  vec3 getCameraPos() { return cameraPos; }
  bool getSample(vec2 *pixel);

private:
  int width, height;
  int fov;
  vec3 cameraPos;
  int x, y;

}

class camera
{
public:
  camera(scene *s);

  ray generateRay();

private:
  vec3 u, v, w;
  scene *scn;
  int fovx, fovy;
}

#endif

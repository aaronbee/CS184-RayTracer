/**
 * Header file for our ray tracer.
 */

#ifndef RAYTRACERH
#define RAYTRACERH

#include "algebra3.h"
#include <string>
#include <vector>

class ray;
class shape;
class scene;
class camera;
class color;
class film;
class raytracer;

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

class camera
{
public:
  camera(scene *s);

  ray generateRay(vec2 pixel);

private:
  vec3 u, v, w;
  scene *scn;
  int fovx, fovy;
}

class scene
{
public:
  scene();
  scene(string path);
  void readScene(string path);
  int getWidth() { return width; }
  int getHeight() { return height; }
  int getFov() { return fov; }
  vec3 getCameraPos() { return cameraPos; }
  bool getSample(vec2 *pixel);

  void render();

private:
  int width, height;
  int fov;
  vec3 cameraPos, cameraUp, cameraLookAt;
  int x, y;
  vector<shape*> objects;
  camera cam;

  void initialparse(FILE *fp);
  void parsefile(FILE *fp);
}

class color
{
public:
  color(vec3 v) : values(v) { }
  color(double r, double g, double b) { values = vec3(r, g, b); }
  vec3 getValues() { return values; }
  double getR() { return values[0]; }
  double getG() { return values[1]; }
  double getB() { return values[2]; }


private:
  vec3 values;
}

class film
{
public:
  film(int width, int height);
  void put(vec2 pixel, color c);
  color get(vec2 pixel);
  void writeToFile(string path);

private:
  vector<vector<color> > *pixels;
}

class raytracer
{
public:
  static color trace(ray r);
}

#endif

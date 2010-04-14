/**
 * Header file for our ray tracer.
 */

#ifndef RAYTRACERH
#define RAYTRACERH

#include "algebra3.h"
#include <string>
#include <vector>

class Ray;
class Shape;
class Scene;
class Camera;
class Color;
class Film;
class RayTracer;

// Global scene
extern Scene *scene;

class Ray
{
public:
  Ray() { }
  Ray(const vec3 p, const vec3 d) : pos(p), dir(d) { }
  vec3 getPos() { return pos; }
  vec3 getDir() { return dir; }

private:
  vec3 pos;
  vec3 dir;
};

class Shape
{
public:
  Shape() { }
  virtual bool intersect(Ray r) { return false; }
};

class Camera
{
public:
  Camera();

  Ray generateRay(vec2 pixel);

private:
  vec3 u, v, w;
  int fovx, fovy;
};

class Scene
{
public:
  Scene(char* path);
  void readScene(char* path);
  int getWidth() { return width; }
  int getHeight() { return height; }
  int getFov() { return fov; }
  vec3 getCameraPos() { return cameraPos; }
  vec3 getCameraUp() { return cameraUp; }
  bool getSample(vec2 *pixel);

  void render();

private:
  int width, height;
  int fov;
  vec3 cameraPos, cameraUp, cameraLookAt;
  int x, y;
  vector<Shape*> *objects;
  Camera *cam;
  RayTracer *rt;
  Film *film;

  void initialparse(FILE *fp);
  void parsefile(FILE *fp);
};

class Color
{
public:
  Color() { };
  Color(vec3 v) : values(v) { }
  Color(double r, double g, double b) { values = vec3(r, g, b); }
  vec3 getValues() { return values; }
  double getR() { return values[0]; }
  double getG() { return values[1]; }
  double getB() { return values[2]; }


private:
  vec3 values;
};

class Film
{
public:
  Film(int width, int height);
  void put(vec2 pixel, Color c);
  Color get(vec2 pixel);
  void writeToFile(string path);

private:
  vector<vector<Color> > pixels;
};

class RayTracer
{
public:
  RayTracer() { }
  Color trace(Ray r);
};

#endif

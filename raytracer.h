/**
 * Header file for our ray tracer.
 */

#ifndef RAYTRACERH
#define RAYTRACERH

#include "algebra3.h"
#include <string>
#include <vector>
#include <sstream>
#include <stack>

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
  Ray transform(const mat4 m) {
	vec3 p, d;

	// Last argument says to drop w
	// if buggy try dealing with homogeneity thing.
	p = vec3(m * vec4(pos, 1), 3);
	d = vec3(m * vec4(dir, 0), 3);
	return Ray(p, d);
  }

private:
  vec3 pos;
  vec3 dir;
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
  string toString(int maxVal) {
    stringstream out;
	out << values[0] * maxVal << " " << values[1] * maxVal << " "
		<< values[2] * maxVal;
	return out.str();
  }

private:
  vec3 values;
};

class Shape
{
public:
  Shape() { }

  virtual vec3 intersect(Ray r) { return false; }
  virtual Color hit(Ray r) { return Color(0,0,0); }

  Color getDiffuse() { return diffuse; }
  Color getSpecular() { return specular; }
  Color getEmission() { return emission; }
  double getShininess() { return shininess; }

protected:
  Color diffuse;
  Color specular;
  Color emission;
  double shininess;
  
  void setMatProps(Color d, Color s, Color e, double sh) {
	diffuse = d;
	specular = s;
	emission = e;
	shininess = sh;
  }

};



class Camera
{
public:
  Camera();

  Ray generateRay(vec2 pixel);

private:
  vec3 u, v, w;
  double fovx, fovy; // in radians
};



//superclass of directional and point lights
class Light
{
public:
  Light(vec3 p, Color c) : pos(p), color(c) { }


  vec3 pos;
  Color color;
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
  vec3 getCameraLookAt() { return cameraLookAt; }
  bool getSample(vec2 *pixel);
  vector<Shape *>* getShapes() { return shapes; }

  vector<Light *>* getLights() { return lights; }

  int getMaxDepth() { return maxDepth; }

  

  void init();
  void render();

private:
  int width, height;
  int fov;
  vec3 cameraPos, cameraUp, cameraLookAt;
  int x, y;
  vector<Shape *> *shapes;
  vector<Light *> *lights;
  
  Camera *cam;
  RayTracer *rt;
  Film *film;
  string outputPath;
  int maxDepth;
  stack<mat4> transformations;

  void initialparse(FILE *fp);
  void parsefile(FILE *fp);
};



class Film
{
public:
  Film();
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

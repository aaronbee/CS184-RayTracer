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

typedef struct 
{
  vec3* vert;
  vec3* norm;
} vertnorm;



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
	p = vec3(m * vec4(pos, 1));
	d = vec3(m * vec4(dir, 0), 3).normalize();
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

  Color operator=(const Color &rhs) {
    if (this == &rhs) {
      return *this;
    }
    values = rhs.values;    
    return *this;
  }
  //TODO: CEILING ON RGB
  Color operator+=(const Color &rhs) {
   values += rhs.values;
   if (values[0] > 1.0) values[0] = 1.0;
   if (values[1] > 1.0) values[1] = 1.0;
   if (values[2] > 1.0) values[2] = 1.0;
   return *this;
  }
  Color operator*=(double scale) {
  values *= scale;
  return *this;
  }
  Color operator+(const Color &other) const {
    Color result = *this;
    result += other;
    return result;
  }
  Color operator*(double scale) const {
    Color result = *this;
    result *= scale;
    return result;
  }
  Color operator*=(const Color &rhs) {
    values = prod(values, rhs.values);
    return *this;
  }
  Color operator*(const Color &other) const {
    Color result = *this;
    result *= other;
    return result;
  }

  void product(const Color other) {
	values = prod(values, other.values);
  }

private:
  vec3 values;
};

class Shape
{
public:
  Shape() { }

  virtual vec3 intersect(Ray r) { return false; }
  Color hit(vec3 intersect);

  Color getDiffuse() { return diffuse; }
  Color getSpecular() { return specular; }
  Color getEmission() { return emission; }
  double getShininess() { return shininess; }

protected:
  Color diffuse;
  Color specular;
  Color emission;
  double shininess;
  
  virtual vec3 calculateNormal(vec3 i) { return vec3(0, 0, 0); }
  vec3 halfAngle(const vec3 one, const vec3 two);

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
  Light() { }

  Color getColor() { return color; }
  virtual double incidentShade(vec3 i, vec3 n) { return 0; }
  virtual vec3 getDirFrom(vec3 i) { return vec3(0, 0, 0); }
  virtual bool blocked(vec3 i) { return false; }
  virtual Color getAttenuatedColor(vec3 i) { return getColor(); }
  
protected:
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
  int getNumSamples() { return numSamples; }
  vector<Shape *>* getShapes() { return shapes; }

  vector<Light *>* getLights() { return lights; }

  int getMaxDepth() { return maxDepth; }
  Color getAmbient() { return ambient; }
  void setAmbient(Color c) { ambient = c; }
  
  void setAttenuation(double c,double l,double q) { attenuation[0] = c; attenuation[1] = l; attenuation[2] = q; }
  
  double getConstantAttenuation() { return attenuation[0]; }
  double getLinearAttenuation() { return attenuation[1]; }
  double getQuadraticAttenuation() { return attenuation[2]; } 
  
  RayTracer *getRayTracer() { return rt; }
  
  void init();
  void render();

private:
  int width, height;
  int fov;
  vec3 cameraPos, cameraUp, cameraLookAt;
  int x, y;
  int numSamples;
  int totalPix;
  vector<Shape *> *shapes;
  vector<Light *> *lights;
  Color ambient;
  double attenuation[3];
  
  Camera *cam;
  RayTracer *rt;
  Film *film;
  string outputPath;
  int maxDepth;
  stack<mat4> transformations;

  void initialparse(FILE *fp);
  void parsefile(FILE *fp);
  void printProgressHeader();
  void printProgress(int count);
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
  double closestHit(Ray r);
};

#endif

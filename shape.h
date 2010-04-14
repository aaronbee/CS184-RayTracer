#include <math.h>
#include "raytracer.h"
#include "algebra3.h"

class Sphere : public Shape {
public:
  Sphere(vec3 c, double r) : center(c), radius(r) { }
  
  /* Check if the ray r intersects the sphere.
   */
  bool intersect(Ray r) {
    // break the ray into position and direction
    // ray p = p_o + p_d (t)
    
    // a sphere is parametrically defined by radius and position
    // (point - Center) * (point - center ) - radius ^2 = 0
    
    double b = 2 * r.getDir() * (r.getPos() - center);
    double c = (r.getPos() - center) * (r.getPos() -center ) - pow(radius,2.0);
    double a = r.getDir() * r.getDir();
    double discrim = pow(b,2.0) - 4 * a * c;
    
    if (discrim < 0.0) { return false; }
    return true;
    
    //a negative?
    //s1 = (-b + sqrt(d))/2a;
    //s2 = (-b - sqrt(d))/2a;
    //two positive: pick smaller
    //both same: tangent
    //one pos one neg: ray origin inside sphere (pick +)
  }
  
private:
  vec3 center;
  double radius;
};


  
class Triangle : public Shape {
public:
  
  /* Check if the ray r intersects the triangle. 
   */
  bool intersect(Ray r) {
    //find the normal (this defines a plane)
    vec3 normal = (b - a) ^ (c - a);
    
    double distance = - ((r.getPos() - a) * normal) / ( r.getDir() * normal);
    
    if (distance < 0.0) { return false; }
    
    //the point where the ray intersects the plane is 
    vec3 p = r.getPos() + distance * r.getDir();
    
    //check if the point is inside the triangle
    if    ((((b - a) ^ (p - a)) * normal >= 0.0)
		   && (((c - b) ^ (p - b)) * normal >= 0.0)
		   && (((a - c) ^ (p - c)) * normal >= 0.0)) {
      return true;
    }
    return false;
  }
  
private:
  vec3 origin;
  vec3 a;
  vec3 b;
  vec3 c;
};

#include <math.h>
#include "raytracer.h"

typedef vector<Light *>::iterator light_itr;

class Sphere : public Shape {
public:
  /* return the intersection point of ray r with self,
   * or NULL if no intersection.
   */
  Sphere(vec3 c, double r, Color d, Color s, Color e, double sh, mat4 m) 
	: center(c), radius(r), matrix(m) {
	setMatProps(d, s, e, sh);
  }

  /* Check if the ray r intersects the sphere.
   */
  vec3 intersect(Ray r) {
    // break the ray into position and direction
    // ray p = p_o + p_d (t)
    
    // a sphere is parametrically defined by radius and position
    // (point - Center) * (point - center ) - radius ^2 = 0
    
    double b = 2 * r.getDir() * (r.getPos() - center);
    double c = (r.getPos() - center) * (r.getPos() -center ) - pow(radius,2.0);
    double a = r.getDir() * r.getDir();
    double discrim = pow(b,2.0) - 4 * a * c;
    
    if (discrim < 0.0) { return NULL; }
    
    double s1 = (-b + sqrt(discrim))/2*a;
    double s2 = (-b - sqrt(discrim))/2*a;
    
    double dist;
    
    if (s1 > 0 && s2 > 0) {
      if (s1 < s2 || s1 == s2) { dist = s1; } 
      else { dist = s2; }
    }
    else if (s1 > 0) { dist = s1; }
    else if (s2 > 0) { dist = s2; }
    else { return NULL; }
    
    //assert(dist > 0.0);
    return r.getPos() + dist * r.getDir();
  }

  /* method hit returns the color of object this seen by ray r
   * preconditions: none
   * Directions:
   * sphereCenter + normalVector = intersectionPoint
   * intersectionPoint + shadowRay = lightPos
   */  
  Color hit(Ray r) {
    vec3 i = intersect(r);
    if (i == NULL) return Color(0,0,0);
    vec3 normal = (i - center).normalize();

    light_itr it = scene->getLights()->begin();
    light_itr end = scene->getLights()->end();

    for ( ; it != end; it ++) {
      vec3 shadowray = (*it)->pos - i;
      shadowray.normalize();
      double colour = shadowray * normal;
      if (colour <= 0.0) return Color(0,0,0);
      return Color(colour, colour, colour);
    }
    return Color(0,0,0);
  }
  
private:
  vec3 center;
  double radius;
  mat4 matrix;
};


  
class Triangle : public Shape {
public:
  Triangle(vec3 _a, vec3 _b, vec3 _c, Color d, Color s, Color e, double sh)
	: a(_a), b(_b), c(_c) { 
	setMatProps(d, s, e, sh);
  }

  /* Check if the ray r intersects the triangle. 
   */
  vec3 intersect(Ray r) {
    //find the normal (this defines a plane)
    vec3 normal = ((b - a) ^ (c - a)).normalize();
  
    
    double distance = - ((r.getPos() - a) * normal) / ( r.getDir() * normal);
    //cout << distance;
    if (distance < 0.0) { return NULL; }
    
    //the point where the ray intersects the plane is 
    vec3 p = r.getPos() + distance * r.getDir();
    
    //check if the point is inside the triangle
    if    ((((b - a) ^ (p - a)) * normal >= 0.0)
		   && (((c - b) ^ (p - b)) * normal >= 0.0)
		   && (((a - c) ^ (p - c)) * normal >= 0.0)) {
      return p;
    }
    return NULL;
  }
  
   Color hit(Ray r) {
    vec3 i = intersect(r);
    if (i == NULL) return Color(0,0,0);
    vec3 normal = ((b-a) ^ (c-a)).normalize();

    light_itr it = scene->getLights()->begin();
    light_itr end = scene->getLights()->end();

    for ( ; it != end; it ++) {
      vec3 shadowray = (*it)->pos - i;
      shadowray.normalize();
      double colour = shadowray * normal;
      if (colour <= 0.0) return Color(0,0,0);
      return Color(colour, colour, colour);
    }
    return Color(0,0,0);
  } 
private:
  vec3 a;
  vec3 b;
  vec3 c;
};

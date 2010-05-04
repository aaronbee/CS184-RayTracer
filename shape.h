#include <math.h>
#include "raytracer.h"


typedef vector<Light *>::iterator light_itr;

Color Shape::hit(vec3 intersect) {
  if (intersect == NULL) return Color(0,0,0);
  vec3 normal = this->calculateNormal(intersect);
  
  light_itr it = scene->getLights()->begin();
  light_itr end = scene->getLights()->end();

  Color result = scene->getAmbient() + emission;

  for ( ; it != end; it ++) {
	Color contrib = Color(0, 0, 0);
	if (!(*it)->blocked(intersect)) {
	  vec3 viewerDir = (scene->getCameraPos() - intersect).normalize();
	  vec3 lightDir = (*it)->getDirFrom(intersect);
	  vec3 h = halfAngle(viewerDir, lightDir);

	  contrib += diffuse * (*it)->incidentShade(intersect, normal);
	  contrib += specular * pow((h * normal), shininess);

	  contrib *= (*it)->getAttenuatedColor(intersect);
	}
	result += contrib;
  }
  return result;
}

/**
 * Returns the normal vector between the two given.
 * Precondition: one and two are normal.
 */
vec3 Shape::halfAngle(const vec3 one, const vec3 two) {
  return (one + two).normalize();
}

class Box : public Shape {
public:
  Box(double x, double X, double y, double Y, double z, double Z) {
	xmin = min(x, X);
	xmax = max(x, X);
	ymin = min(y, Y);
	ymax = max(y, Y);
	zmin = min(z, Z);
	zmax = max(z, Z);
  }

  /**
   * Copied implementation from book.
   */
  vec3 intersect(Ray r) {
	double txmin, txmax, tymin, tymax, tzmin, tzmax;
	double a = 1.0 / r.getDir()[0];
	if (a >= 0) {
	  txmin = a * (xmin - r.getPos()[0]);
	  txmax = a * (xmax - r.getPos()[0]);
	} else {
	  txmin = a * (xmax - r.getPos()[0]);
	  txmax = a * (xmin - r.getPos()[0]);
	}

	a = 1.0 / r.getDir()[1];
	if (a >= 0) {
	  tymin = a * (ymin - r.getPos()[1]);
	  tymax = a * (ymax - r.getPos()[1]);
	} else {
	  tymin = a * (ymax - r.getPos()[1]);
	  tymax = a * (ymin - r.getPos()[1]);
	}

	a = 1.0 / r.getDir()[2];
	if (a >= 0) {
	  tzmin = a * (zmin - r.getPos()[2]);
	  tzmax = a * (zmax - r.getPos()[2]);
	} else {
	  tzmin = a * (zmax - r.getPos()[2]);
	  tzmax = a * (zmin - r.getPos()[2]);
	}

	if ((txmin > tymax) || (tymin > txmax) ||
		(txmin > tzmax) || (tzmin > txmax) ||
		(tymin > tzmax) || (tzmin > tymax))
	  return NULL;
	else
	  return r.getPos() + vec3(r.getDir()[0] * txmin,
							   r.getDir()[1] * tymin,
							   r.getDir()[2] * tzmin);
  }

private:
  double xmin, xmax, ymin, ymax, zmin, zmax;
};

/**
 * Node in Bounding Volume Hierarchy (BVH).
 */
class BVHNode : public Shape {
public:
  vec3 intersect(Ray r) {
	if (bbox.intersect(r) == NULL)
	  return NULL;

	vec3 leftI = NULL;
	vec3 rightI = NULL;
	double distL, distR;
	
	if (left != NULL)
	  leftI = left->intersect(r);

	if (right != NULL)
	  rightI = right->intersect(r);

	if (leftI != NULL && rightI != NULL) {
	  distL = (leftI - scene->getCameraPos()).length();
	  distR = (rightI - scene->getCameraPos()).length();
	  if (distL < distR)
		return leftI;
	  else
		return rightI;

	} else if (leftI != NULL) {
	  return leftI;

	} else if (rightI != NULL) {
	  return rightI;

	} else {
	  return NULL;
	}
  }

private:
  Box bbox;
  Shape *left;
  Shape *right;
};

class Sphere : public Shape {
public:
  /* return the intersection point of ray r with self,
   * or NULL if no intersection.
   */
  Sphere(vec3 c, double r, Color d, Color s, Color e, double sh, mat4 m) 
	: center(c), radius(r), matrix(m) {
	inverse = m.inverse();
	setMatProps(d, s, e, sh);
  }

  /* Check if the ray r intersects the sphere.
   */
  vec3 intersect(Ray r) {
    // break the ray into position and direction
    // ray p = p_o + p_d (t)
    
    // a sphere is parametrically defined by radius and position
    // (point - Center) * (point - center ) - radius ^2 = 0

	Ray transformed = r.transform(inverse);
    
    double b = 2 * transformed.getDir() * (transformed.getPos() - center);
    double c = (transformed.getPos() - center) * (transformed.getPos() -center ) - pow(radius,2.0);
    double a = transformed.getDir() * transformed.getDir();
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
	vec3 i = transformed.getPos() + dist * transformed.getDir();
    return vec3(matrix * vec4(i));
  }

  vec3 calculateNormal(vec3 i) {
	vec3 normal = (vec3(inverse * i) - center);
	normal = vec3(inverse.transpose() * (vec4(normal, 0)), 3);
	normal.normalize();
	return normal;
  }

  /* method hit returns the color of object this seen by ray r
   * preconditions: none
   * Directions:
   * sphereCenter + normalVector = intersectionPoint
   * intersectionPoint + shadowRay = lightPos
   */  
  
private:
  vec3 center;
  double radius;
  mat4 matrix;
  mat4 inverse;
};

class Triangle : public Shape {
public:
  Triangle(vec3 _a, vec3 _b, vec3 _c, Color d, Color s, Color e, double sh)
	: a(_a), b(_b), c(_c) { 
	setMatProps(d, s, e, sh);
  }
  Triangle() {

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

  virtual vec3 calculateNormal(vec3 i) {
	return ((b-a) ^ (c-a)).normalize();
  }
  

protected:
  vec3 a;
  vec3 b;
  vec3 c;
};

class TriNormal : public Triangle {
public: 
  TriNormal(vec3 _a, vec3 _b, vec3 _c, vec3 _na, vec3 _nb, vec3 _nc, Color d, Color s, Color e, double sh) {
    a = _a;
    na = _na;
    b = _b;
    nb = _nb;
    c = _c;
    nc = _nc;
    setMatProps(d,s,e, sh);

  }
  //interpolate the normals
  vec3 calculateNormal(vec3 i) {
    //find the 
    //two sides
    vec3 U = a-b;
    vec3 V = c-b;

    //intersection to point
    vec3 n = i-b;

    double du = U.length();
    double dv = V.length();
    double dn = n.length();

    U.normalize();
    n.normalize();

    double cost = n * U;
    double t = acos(cost); 

    double distx = dn * cos(t);
    double disty = dn * sin(t);

    double u = distx/du;
    double v = disty/dv;


    return vec3(
      (1.0 - u - v) * nb + u * na + v * nc
    ); 
	
  }



private:
  vec3 na;
  vec3 nb;
  vec3 nc;

};


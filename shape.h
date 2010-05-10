#include <math.h>
#include "raytracer.h"


typedef vector<Light *>::iterator light_itr;
typedef vector<Shape *>::iterator shape_itr;

Color Shape::hit(Intersect itrsct) {
  if (!itrsct.isHit()) return Color(0,0,0);
  
  light_itr it = scene->getLights()->begin();
  light_itr end = scene->getLights()->end();

  Color result = scene->getAmbient() + emission;

  for ( ; it != end; it ++) {
	Color contrib = Color(0, 0, 0);
    vec2 offset = vec2((double)rand() / (double)RAND_MAX,
                       (double)rand() / (double)RAND_MAX);

	if (!(*it)->blocked(itrsct.getPos(), offset)) {
	  vec3 viewerDir = (scene->getCameraPos() - itrsct.getPos()).normalize();
	  vec3 lightDir = (*it)->getDirFrom(itrsct.getPos(), offset);
	  vec3 h = halfAngle(viewerDir, lightDir);

	  contrib += diffuse * (*it)->incidentShade(itrsct.getPos(),
												itrsct.getNormal(),
                                                offset);
	  contrib += specular * pow((h * itrsct.getNormal()), shininess);

	  contrib *= (*it)->getAttenuatedColor(itrsct.getPos(), offset);
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

/**
 * Node in Bounding Volume Hierarchy (BVH).
 */
class BVHNode : public Shape {
public:
  BVHNode(Box b, Shape *l, Shape *r) : left(l), right(r) { 
	bbox = b;
  }

  Intersect intersect(Ray r) {
	if (!bbox.isHit(r))
	  return Intersect();

	Intersect leftI = Intersect();
	Intersect rightI = Intersect();
	double distL, distR;
	
	if (left != NULL)
	  leftI = left->intersect(r);

	if (right != NULL)
	  rightI = right->intersect(r);

	if (leftI.isHit() && rightI.isHit()) {
	  distL = (leftI.getPos() - r.getPos()).length();
	  distR = (rightI.getPos() - r.getPos()).length();
	  if (distL < distR)
		return leftI;
	  else
		return rightI;

	} else if (leftI.isHit()) {
	  return leftI;

	} else if (rightI.isHit()) {
	  return rightI;

	} else {
	  return Intersect();
	}
  }

private:
  Shape *left;
  Shape *right;
};

Box combineBoundingBoxes(vector<Shape *> &shapes) {
  shape_itr i = shapes.begin();
  shape_itr end = shapes.end();
  Box b = (*i)->getBoundingBox();
  i ++;
  for (; i != end; i ++) {
	b = b.combine((*i)->getBoundingBox());
  }
  return b;
}

void partition(vector<Shape *> &shapes, vec3 mid, int axis,
			   vector<Shape *> &left, vector<Shape *> &right) {
  shape_itr i = shapes.begin();
  shape_itr end = shapes.end();
  for (; i != end; i ++) {
	if ((*i)->getBoundingBox().midpoint()[axis] < mid[axis])
	  left.push_back(*i);
	else
	  right.push_back(*i);
  }
  if (left.size() == shapes.size()) {
	right.push_back(left.back());
	left.pop_back();
  }
  else if (right.size() == shapes.size()) {
	left.push_back(right.back());
	right.pop_back();
  }
}

BVHNode * createBVHTree(vector<Shape *> &shapes, int axis) {
  int n = shapes.size();
  if (n == 0) {
	return NULL;
  } else if (n == 1) {
	return new BVHNode(shapes[0]->getBoundingBox(), shapes[0], NULL);
  } else if (n == 2) {
	Box b = shapes[0]->getBoundingBox().combine(shapes[1]->getBoundingBox());
	return new BVHNode(b, shapes[0], shapes[1]);
  } else {
	Box b = combineBoundingBoxes(shapes);
	vec3 mid = b.midpoint();
	vector<Shape *> l, r;
	partition(shapes, mid, axis, l, r);
	BVHNode *lt, *rt;
	lt = createBVHTree(l, (axis + 1) % 3);
	rt = createBVHTree(r, (axis + 1) % 3);
	return new BVHNode(b, lt, rt);
  }
}

class Sphere : public Shape {
public:
  /* return the intersection point of ray r with self,
   * or NULL if no intersection.
   */
  Sphere(vec3 c, double r, Color d, Color s, Color e, double sh, double i, mat4 m) 
	: center(c), radius(r), matrix(m) {
	indexOfRefraction = i;
	inverse = m.inverse();
	setMatProps(d, s, e, sh);
	setBoundingBox();
  }

  /* Check if the ray r intersects the sphere.
   */
  Intersect intersect(Ray r) {
    // break the ray into position and direction
    // ray p = p_o + p_d (t)
    
    // a sphere is parametrically defined by radius and position
    // (point - Center) * (point - center ) - radius ^2 = 0

	Ray transformed = r.transform(inverse);
    
    double b = 2 * transformed.getDir() * (transformed.getPos() - center);
    double c = (transformed.getPos() - center) * (transformed.getPos() -center ) - pow(radius,2.0);
    double a = transformed.getDir() * transformed.getDir();
    double discrim = pow(b,2.0) - 4 * a * c;
    
    if (discrim < 0.0) { return Intersect(); }
    
    double s1 = (-b + sqrt(discrim))/2*a;
    double s2 = (-b - sqrt(discrim))/2*a;
    
    double dist;
    
    if (s1 > 0 && s2 > 0) {
      if (s1 < s2 || s1 == s2) { dist = s1; } 
      else { dist = s2; }
    }
    else if (s1 > 0) { dist = s1; }
    else if (s2 > 0) { dist = s2; }
    else { return Intersect(); }
    
    //assert(dist > 0.0);
	vec3 p = transformed.getPos() + dist * transformed.getDir();
	p = vec3(matrix * vec4(p));
	vec3 n = calculateNormal(p);
    return Intersect(p, n, this);
  }

  vec3 calculateNormal(vec3 i) {
	vec3 normal = (vec3(inverse * i) - center);
	normal = vec3(inverse.transpose() * (vec4(normal, 0)), 3);
	normal.normalize();
	return normal;
  }

  void setBoundingBox() {
	vec3 boundings[8];
	boundings[0] = matrix * (center + vec3( radius,  radius,  radius));
	boundings[1] = matrix * (center + vec3( radius,  radius, -radius));
	boundings[2] = matrix * (center + vec3( radius, -radius,  radius));
	boundings[3] = matrix * (center + vec3( radius, -radius, -radius));
	boundings[4] = matrix * (center + vec3(-radius,  radius,  radius));
	boundings[5] = matrix * (center + vec3(-radius,  radius, -radius));
	boundings[6] = matrix * (center + vec3(-radius, -radius,  radius));
	boundings[7] = matrix * (center + vec3(-radius, -radius, -radius));

	vec3 m, M;
	m = boundings[0];
	M = boundings[0];
	for (int i = 0; i < 3; i ++) {
	  for (int j = 1; j < 8; j ++) {
		m[i] = min(m[i], boundings[j][i]);
		M[i] = max(M[i], boundings[j][i]);
	  }
	}
	bbox = Box(m, M);
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
	setBoundingBox();
  }
  Triangle() {

  }

  /* Check if the ray r intersects the triangle. 
   */
  Intersect intersect(Ray r) {
    //find the normal (this defines a plane)
    vec3 normal = ((b - a) ^ (c - a)).normalize();
  
    
    double distance = - ((r.getPos() - a) * normal) / ( r.getDir() * normal);
    //cout << distance;
    if (distance < 0.0) { return Intersect(); }
    
    //the point where the ray intersects the plane is 
    vec3 p = r.getPos() + distance * r.getDir();
    
    //check if the point is inside the triangle
    if ((((b - a) ^ (p - a)) * normal >= 0.0)
		&& (((c - b) ^ (p - b)) * normal >= 0.0)
		&& (((a - c) ^ (p - c)) * normal >= 0.0)) {
      return Intersect(p, calculateNormal(p), this);
    }
    return Intersect();
  }

  virtual vec3 calculateNormal(vec3 i) {
	return ((b-a) ^ (c-a)).normalize();
  }

  void setBoundingBox() {
	vec3 m, M;
	for (int i = 0; i < 3; i ++) {
	  m[i] = min(a[i], min(b[i], c[i]));
	  M[i] = max(a[i], max(b[i], c[i]));
	}
	bbox = Box(m, M);
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
	setBoundingBox();
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


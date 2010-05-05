
#include "raytracer.h"

typedef vector<Shape *>::iterator shape_itr;

Color RayTracer::trace(Ray r, int level)
{
  Color black = Color(0, 0, 0);

  if (level == 0) return black;
 
  shape_itr it = scene->getShapes()->begin();
  shape_itr end = scene->getShapes()->end();

  vec3 pos;
  double dist;
  double closestDist = -1;
  vec3 closestPos;
  Shape *closestShape = NULL;

  for ( ; it != end; it ++) {
	pos = (*it)->intersect(r);
	if (pos != NULL) {
	  dist = (r.getPos() - pos).length();
	  if (closestDist < 0 || dist < closestDist) {
		closestDist = dist;
		closestPos = pos;
		closestShape = *it;
	  }
	}
  }
  
  if (closestShape != NULL) {
	vec3 theNormal = closestShape->calculateNormal(closestPos);
	vec3 vpar = (theNormal * r.getDir().normalize()) * theNormal;
	vec3 ref = (r.getDir() - 2*vpar).normalize();
	return closestShape->hit(closestPos) + closestShape->getSpecular() * trace(Ray(closestPos + 0.01 * ref, ref), level-1);
  }
  
  return black;
}

double RayTracer::closestHit(Ray r) {
  vec3 i = scene->getTree()->intersect(r);
  if (i == NULL) return -1;
  return (r.getPos() - i).length();
}

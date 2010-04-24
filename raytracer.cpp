
#include "raytracer.h"

typedef vector<Shape *>::iterator shape_itr;

Color RayTracer::trace(Ray r)
{
  Color black = Color(0, 0, 0);

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
	return closestShape->hit(closestPos);
  }
  
  return black;
}

double RayTracer::closestHit(Ray r) {
  double closestDist = -1;

  shape_itr it = scene->getShapes()->begin();
  shape_itr end = scene->getShapes()->end();

  vec3 pos;
  double dist;

  for (; it != end; it ++) {
	pos = (*it)->intersect(r);
	if (pos != NULL) {
	  dist = (r.getPos() - pos).length();
	  if (closestDist < 0 || dist < closestDist) {
		closestDist = dist;
	  }
	}
  }
  return closestDist;
}

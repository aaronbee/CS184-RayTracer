
#include "raytracer.h"

typedef vector<Shape *>::iterator shape_itr;

Color RayTracer::trace(Ray r)
{
  Color red = Color(1.0, 0, 0);
  Color black = Color(0, 0, 0);

  shape_itr it = scene->getShapes()->begin();
  shape_itr end = scene->getShapes()->end();

  for ( ; it != end; it ++) {
	//if ((*it)->intersect(r) != NULL) {
	//	return red;
	//  }
    return (*it)->hit(r);
  }
  
  return black;
}

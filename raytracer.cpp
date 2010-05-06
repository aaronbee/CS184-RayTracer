
#include "raytracer.h"

typedef vector<Shape *>::iterator shape_itr;

Color RayTracer::trace(Ray r, int level)
{
  Color black = Color(0, 0, 0);

  if (level == 0) return black;
 
  shape_itr it = scene->getShapes()->begin();
  shape_itr end = scene->getShapes()->end();

  Intersect itrsct = closestHit(r);
  
  if (itrsct.isHit()) {
	vec3 theNormal = itrsct.getNormal();
	vec3 vpar = (theNormal * r.getDir().normalize()) * theNormal;
	vec3 ref = (r.getDir() - 2*vpar).normalize();
	return itrsct.getShape()->hit(itrsct) + itrsct.getShape()->getSpecular() * trace(Ray(itrsct.getPos() + 0.01 * ref, ref), level-1);
  }
  
  return black;
}

Intersect RayTracer::closestHit(Ray r) {
  return scene->getTree()->intersect(r);
}

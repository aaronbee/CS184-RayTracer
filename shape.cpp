#include "raytracer.h"

typedef vector<Light *>::iterator light_itr;

Color Shape::hit(vec3 intersect) {
  if (intersect == NULL) return Color(0,0,0);
  vec3 normal = this->calculateNormal(intersect);
  
  light_itr it = scene->getLights()->begin();
  light_itr end = scene->getLights()->end();

  Color result = scene->getAmbient();

  for ( ; it != end; it ++) {
	result += (*it)->incidentShade(intersect, normal);
  }
  return result;
}


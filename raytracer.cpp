
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
	vec3 n = itrsct.getNormal();

	//the computation for the reflected ray.
	vec3 vpar = (n * r.getDir().normalize()) * n;
	vec3 ref = (r.getDir() - 2*vpar).normalize();

	//shortcircuit for non-transparent objects
	if( itrsct.getShape()->getIndex() == 0.0) {
	  return itrsct.getShape()->hit(itrsct) + itrsct.getShape()->getSpecular() * trace(Ray(itrsct.getPos() + 0.01 * ref, ref), level-1);
	}

	//the computation for the refracted ray.
	//get index of refraction for current ray
	//the ray then assumes the index of refraction for the surface being entered
	vec3 d = r.getDir().normalize();
	double index1 = r.getIndex();
	double index2 = itrsct.getShape()->getIndex();
	//if (index1 == 1.0) { index2 = 1.5; }
	//else {index2 = 1.0; n = -n; }
	if(index1 != 1.0) { n = -n; index2 = 1.0; }

	vec3 term1 = index1 * (d - n * (d * n)) / index2;
	double term3 = 1.0 - pow((d * n),2);
	double undersqrt =  ( 1.0 - (   pow(index1, 2) * term3   /  pow(index2,2)   ) );
	vec3 term2 = n * (sqrt(undersqrt));
	vec3 transmission =  (term1 - term2).normalize();
	
	Color refracted; 
	if (undersqrt < 0 ) {
	  //case of total internal reflection
	  refracted = black;
	}
	else {
	  refracted = trace(Ray(itrsct.getPos()+0.01 * transmission, transmission, index2), level-1);
	  	
	  double distance = (itrsct.getPos() - r.getPos()).length();
	  refracted = refracted * exp(- log(1.1) * distance);
	}

	return itrsct.getShape()->hit(itrsct) + itrsct.getShape()->getSpecular() * trace(Ray(itrsct.getPos() + 0.01 * ref, ref), level-1) + refracted;

  }
  
  return black;
}

Intersect RayTracer::closestHit(Ray r) {
  return scene->getTree()->intersect(r);
}

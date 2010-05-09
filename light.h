#include "raytracer.h"

#define OFFSET 0.2

class PointLight : public Light {
public:
  PointLight(vec3 p, Color c, double *atten) {
    pos = p;
    color = c;
	attenuation = atten;
  }
  
  vec3 getPos() { return pos; }

  double incidentShade(vec3 i, vec3 normal, vec2 offset) {
    vec3 shadowray = pos - i;
    shadowray.normalize();
    double colour = shadowray * normal;
    if (colour <= 0.0) return 0;
    return colour;
  }

  vec3 getDirFrom(vec3 i, vec2 offset) {
	return (pos - i).normalize();
  }

  bool blocked(vec3 i, vec2 offset) {
	vec3 direction = getDirFrom(i, offset);
	Ray r = Ray(i + (direction * OFFSET), direction);
	Intersect itrsct = scene->getRayTracer()->closestHit(r);
	double hitDist = (itrsct.getPos() - i).length();
	if (!itrsct.isHit()) {
	  return false;
	}
	double dist = (i - pos).length();
	if (hitDist < dist) {
	  return true;
	}
	return false;
  }

  Color getAttenuatedColor(vec3 i, vec2 offset) {
	double dist = (pos - i).length();
	Color c = getColor();
	double atten = attenuation[0] + attenuation[1] * dist +
	  attenuation[2] * pow(dist, 2);
	atten = 1 / atten;
	return getColor() * atten;
  }

private:  
  vec3 pos;
  double *attenuation;
};

class DirectionalLight : public Light {
public:
  DirectionalLight (vec3 d, Color c) {
    dir = d;
    color = c;
  }

  vec3 getDir() { return dir; }

  double incidentShade(vec3 i, vec3 normal, vec2 offset) {
    vec3 shadowray = dir;
    shadowray.normalize();
    double colour = shadowray * normal;
    if (colour <= 0.0) return 0;
    return colour;
  }

  vec3 getDirFrom(vec3 i, vec2 offset) {
	return dir.normalize();
  }

  bool blocked(vec3 i, vec2 offset) {
	vec3 direction = getDirFrom(i, offset);
	Ray r = Ray(i + (direction * OFFSET), direction);
	Intersect itrsct = scene->getRayTracer()->closestHit(r);
	if (itrsct.isHit()) {
	  return true;
	} else {
	  return false;
	}
  }

private:
  vec3 dir;
};

class AreaLight : public Light {
public:
  AreaLight(vec3 pos, double x, double y, Color c) {
    corner = pos;
    color = c;
    width = x;
    length = y;
  }

  double incidentShade(vec3 i, vec3 normal, vec2 offset) {
    return 0;
  }
  

private:
  vec3 corner;
  double width;
  double length;
};

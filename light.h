#include "raytracer.h"

class PointLight : public Light {
public:
  PointLight(vec3 p, Color c) {
    pos = p;
    color = c;
  }
  
  vec3 getPos() { return pos; }

  double incidentShade(vec3 i, vec3 normal) {
    vec3 shadowray = pos - i;
    shadowray.normalize();
    double colour = shadowray * normal;
    if (colour <= 0.0) return 0;
    return colour;
  }

  vec3 getDirFrom(vec3 i) {
	return (pos - i).normalize();
  }

  bool blocked(vec3 i) {
	vec3 direction = getDirFrom(i);
	Ray r = Ray(i + (direction * 0.1), direction);
	double hitDist = scene->getRayTracer()->closestHit(r);
	if (hitDist < 0) {
	  return false;
	}
	double dist = (i - pos).length();
	if (hitDist < dist) {
	  return true;
	}
	return false;
  }

private:  
  vec3 pos;
};

class DirectionalLight : public Light {
public:
  DirectionalLight (vec3 d, Color c) {
    dir = d;
    color = c;
  }

  vec3 getDir() { return dir; }

  double incidentShade(vec3 i, vec3 normal) {
    vec3 shadowray = dir;
    shadowray.normalize();
    double colour = shadowray * normal;
    if (colour <= 0.0) return 0;
    return colour;
  }

  vec3 getDirFrom(vec3 i) {
	return dir.normalize();
  }

  bool blocked(vec3 i) {
	vec3 direction = getDirFrom(i);
	Ray r = Ray(i + (direction * 0.1), direction);
	double hitDist = scene->getRayTracer()->closestHit(r);
	if (hitDist < 0) {
	  return false;
	} else {
	  return true;
	}
  }

private:
  vec3 dir;
};


#include "raytracer.h"

#define OFFSET 0.2

class PointLight : public Light {
public:
  PointLight();
  PointLight(vec3 p, Color c, double *atten) {
    pos = p;
    color = c;
	attenuation = atten;
  }
  
  virtual vec3 getPos(vec2 offset) { return pos; }

  double incidentShade(vec3 i, vec3 normal, vec2 offset) {
    vec3 shadowray = getPos(offset) - i;
    shadowray.normalize();
    double colour = shadowray * normal;
    if (colour <= 0.0) return 0;
    return colour;
  }

  vec3 getDirFrom(vec3 i, vec2 offset) {
	return (getPos(offset) - i).normalize();
  }

  bool blocked(vec3 i, vec2 offset) {
	vec3 direction = getDirFrom(i, offset);
	Ray r = Ray(i + (direction * OFFSET), direction);
	Intersect itrsct = scene->getRayTracer()->closestHit(r);
	double hitDist = (itrsct.getPos() - i).length();
	if (!itrsct.isHit()) {
	  return false;
	}
	double dist = (i - getPos(offset)).length();
	if (hitDist < dist) {
	  return true;
	}
	return false;
  }

  Color getAttenuatedColor(vec3 i, vec2 offset) {
	double dist = (getPos(offset) - i).length();
	Color c = getColor();
	double atten = attenuation[0] + attenuation[1] * dist +
	  attenuation[2] * pow(dist, 2);
	atten = 1 / atten;
	return getColor() * atten;
  }

protected:  
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

class AreaLight : public PointLight {
public:
  AreaLight(vec3 p, double x, double y, Color c, double *atten) {
    pos = p;
    color = c;
    width = x;
    length = y;
    attenuation = atten;
  }
  
  vec3 getPos(vec2 offset) {
    return pos + vec3(width * offset[0], length * offset[1], 0);
  }

private:
  double width;
  double length;
};

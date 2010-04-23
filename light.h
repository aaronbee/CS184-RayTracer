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
    if (colour <= 0.0) return Color(0,0,0);
    return colour;
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
    if (colour <= 0.0) return Color(0,0,0);
    return colour;
  }


private:
  vec3 dir;
};


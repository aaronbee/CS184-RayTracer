#include "raytracer.h"

class PointLight : public Light {
public:
  //PointLight(vec3 p, Color c) : pos(p), color(c) { }
  
  vec3 pos;
  Color color;
};

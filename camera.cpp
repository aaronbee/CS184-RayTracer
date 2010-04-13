
#include "raytracer.h"

camera::camera(scene &s)
{
  w = s.cameraPos().normalize();
  u = w ^ cameraUp();
  u.normalize();
  v = w ^ u;
}

ray camera::generateRay()
{
  
}

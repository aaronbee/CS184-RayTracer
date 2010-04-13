
#include "raytracer.h"
#incluce <math.h>

camera::camera(scene *s) : scn(s)
{
  w = scn->getCameraPos().normalize();
  u = w ^ cameraUp();
  u.normalize();
  v = w ^ u;

  fovy = scn->getFov();
  fovx = ((double) scn->getWidth() / (double) scn->getHeight()) * fovy;
}

ray camera::generateRay(vec2 pixel)
{
  double alpha, beta;
  vec3 dir;
  alpha = tan(fovx / 2) * (pixel[0] - (width / 2)) / (width / 2);
  beta = tan(fovy / 2) * ((height / 2) - pixel[1]) / (height / 2);

  dir = (alpha * u) + (beta * v) - w;
  dir.normalize();
  dir += scn.getCameraPos();

  return ray(scn->getCameraPos(), dir);
}

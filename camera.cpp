
#include "raytracer.h"
#include <math.h>

Camera::Camera()
{
  w = (scene->getCameraPos() - scene->getCameraLookAt()).normalize();
  u = w ^ scene->getCameraUp();
  u.normalize();
  v = w ^ u;

  fovy = scene->getFov();
  fovx = ((double) scene->getWidth() / (double) scene->getHeight()) * fovy;
}

Ray Camera::generateRay(vec2 pixel)
{
  double alpha, beta;
  vec3 dir;
  int width = scene->getWidth();
  int height = scene->getHeight();

  double fovXRad = fovx * M_PI / 180.0;
  double fovYRad = fovy * M_PI / 180.0;
  
  alpha = - tan(fovXRad / 2) * (pixel[0] - (width / 2)) / (width / 2);
  beta = tan(fovYRad / 2) * ((height / 2) - pixel[1]) / (height / 2);

  dir = (alpha * u) + (beta * v) - w;
  dir.normalize();
  //  dir += scene->getCameraPos();
  //  dir.normalize();

  Ray rtn = Ray(scene->getCameraPos(), dir);

  return rtn;
}

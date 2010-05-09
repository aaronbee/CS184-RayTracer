
#include "raytracer.h"
#include <math.h>

Camera::Camera()
{
  w = (scene->getCameraPos() - scene->getCameraLookAt()).normalize();
  u = w ^ scene->getCameraUp();
  u.normalize();
  v = w ^ u;

  fovy = scene->getFov() * M_PI / 180.0;
  double z = tan(fovy / 2);
  if (z == 0) {
	exit(1);
  }
  z = (1 / z) * scene->getHeight() / 2;

  fovx = 2 * atan(scene->getWidth() / 2 / z);
}

Ray Camera::generateRay(vec2 pixel)
{
  double alpha, beta;
  vec3 dir;
  int width = scene->getWidth();
  int height = scene->getHeight();

  if (scene->getNumSamples() > 1) {
	pixel[0] += ((double)rand() / (double)RAND_MAX) - 0.5;
	pixel[1] += ((double)rand() / (double)RAND_MAX) - 0.5;
  }

  alpha = - tan(fovx / 2) * (pixel[0] - (width / 2)) / (width / 2);
  beta = tan(fovy / 2) * ((height / 2) - pixel[1]) / (height / 2);

  dir = (alpha * u) + (beta * v) - w;
  dir.normalize();

  Ray rtn = Ray(scene->getCameraPos(), dir);

  return rtn;
}

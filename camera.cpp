
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

  vec3 origin = scene->getCameraPos();
  vec3 wTemp = w;
  vec3 uTemp = u;
  vec3 vTemp = v;

  if (scene->getNumSamples() > 1) {
	pixel[0] += ((double)rand() / (double)RAND_MAX) - 0.5;
	pixel[1] += ((double)rand() / (double)RAND_MAX) - 0.5;
    origin += u * (scene->getLensSize() *
                   (((double)rand() / (double)RAND_MAX) - 0.5));
    origin += v * (scene->getLensSize() *
                   (((double)rand() / (double)RAND_MAX) - 0.5));

    wTemp = (origin - scene->getCameraLookAt()).normalize();
    uTemp = wTemp ^ scene->getCameraUp();
    uTemp.normalize();
    vTemp = wTemp ^ uTemp;
  }

  alpha = - tan(fovx / 2) * (pixel[0] - (width / 2)) / (width / 2);
  beta = tan(fovy / 2) * ((height / 2) - pixel[1]) / (height / 2);

  dir = (alpha * uTemp) + (beta * vTemp) - wTemp;
  dir.normalize();

  return Ray(origin, dir);
}

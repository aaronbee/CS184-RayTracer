
#include "raytracer.h"

film::film(int width, int height)
{
  pixels = vector(width, vector(height));
}

void film::put(vec2 pixel, color c)
{
  pixels[pixel[0]][pixel[1]] = c;
}

color film::get(vec2 pixel)
{
  return pixels[pixel[0]][pixel[1]];
}

void film::writeToFile(string path)
{

}

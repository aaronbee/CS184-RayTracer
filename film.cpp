
#include "raytracer.h"

Film::Film(int width, int height)
{
  pixels = vector<vector<Color> >(width, vector<Color>(height));
}

void Film::put(vec2 pixel, Color c)
{
  pixels[pixel[0]][pixel[1]] = c;
}

Color Film::get(vec2 pixel)
{
  return pixels[pixel[0]][pixel[1]];
}

void Film::writeToFile(string path)
{

}


#include "raytracer.h"

Film::Film()
{
  pixels = vector<vector<Color> >(scene->getHeight(),
	vector<Color>(scene->getWidth()));
}

void Film::put(vec2 pixel, Color c)
{
  pixels[pixel[1]][pixel[0]] = c;
}

Color Film::get(vec2 pixel)
{
  return pixels[pixel[1]][pixel[0]];
}

void Film::writeToFile(string path)
{
  typedef vector<Color>::iterator itr;
  int maxVal = 255;

  cout << "P3" << endl;
  cout << scene->getWidth() << " " << scene->getHeight() << endl;
  cout << maxVal << endl;

  for (int i = 0; i < scene->getHeight(); i ++) {
	for (itr color = pixels[i].begin(); color != pixels[i].end(); color ++) {
	  cout << color->toString(maxVal) << " ";
	}
	cout << endl;
  }
}

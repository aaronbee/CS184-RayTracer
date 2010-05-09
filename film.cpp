
#include "raytracer.h"
#include <FreeImage.h>

Film::Film()
{
  pixels = vector<vector<Color> >(scene->getHeight(),
								  vector<Color>(scene->getWidth(), Color(0, 0, 0)));
}

void Film::put(vec2 pixel, Color c)
{
  pixels[pixel[1]][pixel[0]] += (c * (1.0 / scene->getNumSamples()));
}

Color Film::get(vec2 pixel)
{
  return pixels[pixel[1]][pixel[0]];
}

void Film::writeToFile(string path)
{
  typedef vector<Color>::iterator itr;
  int maxVal = 255;
  int bpp = 24;
  FreeImage_Initialise();
  FIBITMAP* bitmap = FreeImage_Allocate(scene->getWidth(),
										scene->getHeight(),
										bpp);
  RGBQUAD rgb;

  if (!bitmap) {
	cerr << "FreeImage couldn't allocate an image." << endl;
	exit(1);
  }

  for (int y = 0; y < scene->getHeight(); y ++) {
	vector<Color> row = pixels[y];
	for (int x = 0; x < scene->getWidth(); x ++) {
	  Color color = row[x];
	  rgb.rgbRed = color.getR() * maxVal;
	  rgb.rgbGreen = color.getG() * maxVal;
	  rgb.rgbBlue = color.getB() * maxVal;

	  FreeImage_SetPixelColor(bitmap, x, y, &rgb);
	}
  }
	 
  if (FreeImage_Save(FIF_PNG, bitmap, path.c_str(), 0)) {
	cout << "Image saved to " << path << endl;
  }
  else {
	cerr << "Couldn't save image to " << path << endl;
	exit(1);
  }

  FreeImage_DeInitialise();
}

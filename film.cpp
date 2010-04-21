
#include "raytracer.h"
#include "FreeImage.h"

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
  int bpp = 24;
  FreeImage_Initialise();
  FIBITMAP* bitmap = FreeImage_Allocate(scene->getWidth(),
										scene->getHeight(),
										24);
  RGBQUAD rgb;

  if (!bitmap) {
	cerr << "FreeImage couldn't allocate an image." << endl;
	exit(1);
  }

  for (int y = 0; y < scene->getHeight(); y ++) {
	vector<Color> row = pixels[y];
	for (int x = 0; x < scene->getWidth(); x ++) {
	  Color color = row[x];
	  rgb.rgbRed = color.getR();
	  rgb.rgbGreen = color.getG();
	  rgb.rgbBlue = color.getB();

	  FreeImage_SetPixelColor(bitmap, x, y, &rgb);
	}
  }
	 
  if (FreeImage_Save(FIF_PNG, bitmap, path, 0)) {
	cout << "Image saved to " << path << endl;
  }
  else {
	cerr << "Couldn't save image to " << path << endl;
	exit(1);
  }

  FreeImage_DeInitialise();
  /*
  cout << "P3" << endl;
  cout << scene->getWidth() << " " << scene->getHeight() << endl;
  cout << maxVal << endl;

	  cout << color->toString(maxVal) << " ";
	}
	cout << endl;
	}*/
  
  
}

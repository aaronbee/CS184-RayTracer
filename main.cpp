
#include "raytracer.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "FreeImage.h"

Scene *scene;

int main(int argc, char *argv[]) {
  /*  scene = new Scene(argv[1]);
  scene->init();
  scene->render();*/

  FreeImage_Initialise();
  cout << "FreeImage " << FreeImage_GetVersion() << endl;
  cout << FreeImage_GetCopyrightMessage() << endl << endl;
  FreeImage_DeInitialise();
}

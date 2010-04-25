
#include "raytracer.h"
#include <stdlib.h>
#include <stdio.h>

Scene *scene;

int main(int argc, char *argv[]) {
  srand(1);
  scene = new Scene(argv[1]);
  scene->init();
  scene->render();
}

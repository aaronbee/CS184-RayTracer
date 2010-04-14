
#include "raytracer.h"
#include <stdlib.h>
#include <stdio.h>

extern Scene *scene;

int main(int argc, char *argv[]) {
  scene = new Scene(argv[1]);
}

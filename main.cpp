
#include "raytracer.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  scene s(argv[1]);
  camera c(&s);
}

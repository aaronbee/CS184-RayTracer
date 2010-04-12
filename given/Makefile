TOP = -L/usr/X11R6/lib

GLUT = -lglut
LLDLIBS = $(GLUT) -lGLU -lGL -lXmu -lXext -lX11 -lpthread -lm

.cpp.o:
	g++ -g -c  $<

all: openglviewer

openglviewer: openglviewer.o
	g++ -g $< -o $@ $(TOP) $(LLDLIBS)

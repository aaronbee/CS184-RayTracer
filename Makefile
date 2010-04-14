CC = g++
CFLAGS = -Wall -g
LDFLAGS = -lm

all : raytracer

camera.o : camera.cpp raytracer.h
	${CC} ${CFLAGS} -c camera.cpp

film.o : film.cpp raytracer.h
	${CC} ${CFLAGS} -c film.cpp

scene.o : scene.cpp raytracer.h
	${CC} ${CFLAGS} -c scene.cpp

raytracer.o : raytracer.cpp raytracer.h
	${CC} ${CFLAGS} -c raytracer.cpp

main.o : main.cpp raytracer.h
	${CC} ${CFLAGS} -c main.cpp

raytracer : main.o camera.o film.o scene.o raytracer.o
	${CC} ${CFLAGS} main.o camera.o film.o scene.o raytracer.o ${LDFLAGS} -o raytracer

clean :
	rm -rf *o *~ raytracer
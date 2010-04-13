CC = g++
CFLAGS = -Wall -g
LDFLAGS = -lm

camera.o : camera.cpp raytracer.h
	${CC} ${CFLAGS} -c camera.cpp

film.o : film.cpp raytracer.h
	${CC} ${CFLAGS} -c camera.cpp

scene.o : scene.cpp raytracer.h
	${CC} ${CFLAGS} -c scene.cpp

main.o : main.cpp raytracer.h
	${CC} ${CFLAGS} -c main.cpp

raytracer : main.o camera.o film.o scene.o
	${CC} ${CFLAGS} main.o camera.o film.o scene.o ${LDFLAGS} -o raytracer
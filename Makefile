CC = g++
CFLAGS = -Wall -g -I/opt/local/include
LDFLAGS = -L/opt/local/lib -lm -lfreeimage

all : raytracer

camera.o : camera.cpp raytracer.h
	${CC} ${CFLAGS} -c camera.cpp

film.o : film.cpp raytracer.h
	${CC} ${CFLAGS} -c film.cpp

scene.o : scene.cpp raytracer.h shape.h light.h
	${CC} ${CFLAGS} -c scene.cpp

raytracer.o : raytracer.cpp raytracer.h
	${CC} ${CFLAGS} -c raytracer.cpp

shape.o : shape.cpp shape.h raytracer.h
	${CC} ${CFLAGS} -c shape.cpp

main.o : main.cpp raytracer.h
	${CC} ${CFLAGS} -c main.cpp

raytracer : main.o camera.o film.o scene.o raytracer.o shape.o
	${CC} ${CFLAGS} main.o camera.o film.o shape.o scene.o raytracer.o ${LDFLAGS} -o raytracer

clean :
	rm -rf *o *~ raytracer
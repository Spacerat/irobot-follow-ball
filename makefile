CC=gcc
CFLAGS=

robot_test: robot_test.o robot.o delay.o
	gcc robot_test.c robot.c delay.c -o robot_test
	
robot_cmd: robot_cmd.o robot.o delay.o cbuffer.o
	gcc robot_cmd.c robot.c delay.c cbuffer.c -o robot_cmd -lpthread
	
vision_test:
	qmake -o qmakefile QtOpenCV.pro
	make -f qmakefile
	
robot.o: robot.c delay.o
	gcc -c robot.c delay.c
	
delay.o: delay.c
	gcc -c delay.c
	
cbuffer.o: cbuffer.c
	gcc -c cbuffer.c -lpthread
	
clean:
	rm -rf *.o

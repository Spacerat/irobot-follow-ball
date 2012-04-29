CC=gcc
CFLAGS=

robot_main: robot_main.c delay.o robot.o robot_threaded.o vision.o  
	gcc robot_main.c robot_threaded.o delay.o robot.o vision.o -o robot_main -lpthread

robot_test: robot_test.c robot.o delay.o
	gcc robot_test.c robot.o delay.o -o robot_test
	
robot_cmd: robot_cmd.c robot.o delay.o cbuffer.o
	gcc robot_cmd.c robot.o delay.o cbuffer.o -o robot_cmd -lpthread
	
vision_test:
	qmake -o qmakefile vision_test.pro
	make -f qmakefile
	
robot_threaded.o: robot_threaded.c robot.o
	gcc -c robot_threaded.c robot.o
	
vision.o: vision.c
	gcc -c vision.c -lpthread

robot.o: robot.c delay.o
	gcc -c robot.c delay.o
	
delay.o: delay.c
	gcc -c delay.c
	
cbuffer.o: cbuffer.c
	gcc -c cbuffer.c -lpthread
	
clean:
	rm -rf *.o

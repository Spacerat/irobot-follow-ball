CC=gcc
CFLAGS=

robot_test: robot_test.o robot.o delay.o
	gcc robot_test.c robot.c delay.c -o robot_test
	
robot.o: robot.c delay.o
	gcc -c robot.c delay.c
	
delay.o: delay.c
	gcc -c delay.c
	
clean:
	rm -rf *.o

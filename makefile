CC=gcc
CFLAGS=

all: robot_main vision_test robot_cmd robot_test
	

robot_main: robot_main.c delay.o robot.o robot_threaded.o vision.o  
	gcc robot_main.c robot_threaded.o delay.o robot.o vision.o /usr/lib/libcxcore.so.2.1  -lcv -lhighgui  -lpthread -o robot_main

robot_test: robot_test.c robot.o delay.o
	gcc robot_test.c robot.o delay.o -o robot_test
	
robot_cmd: robot_cmd.c robot.o delay.o cbuffer.o
	gcc robot_cmd.c robot.o delay.o cbuffer.o -o robot_cmd -lpthread
	
vision_test: vision_test.cpp
	qmake -o qmakefile vision_test.pro
	make -f qmakefile
	
ui_test: vision_test.cpp MyCameraWindow.cpp QOpenCVWidget.cpp
	qmake -o qmakefile ui_test.pro
	make -f qmakefile

robot_threaded.o: robot_threaded.c robot.o
	gcc -c robot_threaded.c robot.o
	
vision.o: vision.c
	gcc -c vision.c -lpthread -lc -lhighgui -lstdc++

robot.o: robot.c delay.o
	gcc -c robot.c delay.o
	
delay.o: delay.c
	gcc -c delay.c
	
cbuffer.o: cbuffer.c
	gcc -c cbuffer.c -lpthread
	
clean:
	rm -rf *.o
	

CC = g++
CCPARAM = -Isrc/header -Iext/header -Iext/bullet -Lext/linux -lglew -lGL -lglfw3 -lpthread -lX11 -ldl -std=c++17

all: tmp/window.o tmp/camera.o

tmp/window.o:
	$(CC) -c src/source/graphics/window/window.cpp -o tmp/window.o  $(CCPARAM)

tmp/camera.o:
	$(CC) -c src/source/graphics/camera/camera.cpp -o tmp/camera.o $(CCPARAM)
	

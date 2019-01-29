CC = g++
MYINCLUDES = -I/home/benny/Desktop/folder/citrus/src/header
EXTINCLUDES = -I/usr/include -I/usr/include/bullet -I/home/benny/Desktop/vulkan/1.1.92.1/x86_64/include -I/usr/include/libpng
EXTLIBS = \
-L/home/benny/Desktop/vulkan/1.1.92.1/x86_64/lib \
-L/usr/lib/x86_64-linux-gnu \
-L/home/benny/Desktop/bullet3build/src/BulletCollision \
-L/home/benny/Desktop/bullet3build/src/BulletDynamics \
-L/home/benny/Desktop/bullet3build/src/LinearMath
DEFINES = -DBT_USE_DOUBLE_PRECISION
FLAGS = -g
LINKS = -lassimp -lGLEW -lGL -lglfw -lpthread -lX11 -ldl -std=c++17 -lvulkan -lpng -lBulletCollision -lBulletDynamics -lLinearMath -lnfd -lgtk-3 -lglib-2.0 -lgobject-2.0
CCPARAM = $(MYINCLUDES) $(FLAGS) $(EXTINCLUDES) $(EXTLIBS) $(LINKS)

SRC_DIR = /home/benny/Desktop/folder/citrus/src/source
BUILD_DIR = ./build
OFILES_DIR = $(BUILD_DIR)/source
EXECUTABLE = $(BUILD_DIR)/ctvk.exe

$(shell rsync -a --include '*/' --exclude '*' "$(SRC_DIR)" "$(BUILD_DIR)")

CPP_FILES = $(shell find $(SRC_DIR) | grep \.cpp)
OFILES = $(foreach line, $(CPP_FILES), $(patsubst $(SRC_DIR)/%.cpp,$(OFILES_DIR)/%.o,$(line)))

define OFILE_TEMPLATE

$(patsubst \,$(space),$(shell $(CC) $(INCLUDE_FLAGS) -MM $(1) -MT $(patsubst $(SRC_DIR)/%.cpp,$(OFILES_DIR)/%.o,$(1))))
	$(CC) -c -o $$@ $$< $(CCPARAM) 

endef

OFILE_TARGETS=$(foreach file,$(CPP_FILES), $(call OFILE_TEMPLATE,$(file)))

.PHONY : clean

all: $(EXECUTABLE)

$(eval $(OFILE_TARGETS))

$(EXECUTABLE) : $(OFILES)
	$(CC) -o $(EXECUTABLE) $(OFILES) $(CCPARAM) 

clean : 
	rm -rf $(BUILD_DIR)/*

test : all
	LD_LIBRARY_PATH=/home/benny/Desktop/vulkan/1.1.92.1/x86_64/lib VK_LAYER_PATH=/home/benny/Desktop/vulkan/1.1.92.1/x86_64/etc/explicit_layer.d $(EXECUTABLE)

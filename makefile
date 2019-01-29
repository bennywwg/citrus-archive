VULKAN_SDK_PATH = /home/benny/Desktop/vulkan/1.1.92.1/x86_64
BULLET_BUILD_PATH = /home/benny/Desktop/bullet3build

CC = g++
MYINCLUDES = -Isrc/header
EXTINCLUDES = -I$(VULKAN_SDK_PATH)/include
EXTLIBS = \
-L$(VULKAN_SDK_PATH)/lib \
-L$(BULLET_BUILD_PATH)/src/BulletCollision \
-L$(BULLET_BUILD_PATH)/src/BulletDynamics \
-L$(BULLET_BUILD_PATH)/src/LinearMath
DEFINES = -DBT_USE_DOUBLE_PRECISION
FLAGS = -g
LINKS = -lassimp -lGLEW -lGL -lglfw -lpthread -lX11 -ldl -std=c++17 -lvulkan -lpng -lBulletCollision -lBulletDynamics -lLinearMath -lnfd -lgtk-3 -lglib-2.0 -lgobject-2.0
CCPARAM = $(MYINCLUDES) $(FLAGS) $(EXTINCLUDES) $(EXTLIBS) $(LINKS)

SRC_DIR = src/source
BUILD_DIR = build
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
	LD_LIBRARY_PATH=$(VULKAN_SDK_PATH)/lib VK_LAYER_PATH=$(BULLET_SDK_PATH)/etc/explicit_layer.d $(EXECUTABLE)

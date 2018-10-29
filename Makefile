CC = g++
CCPARAM = -Isrc/header -Iext/header -I/usr/include/bullet -I/usr/include/libpng -lglew -lGL -lglfw3 -lpthread -lX11 -ldl -std=c++17 -lstdc++fs

SRC_DIR = src/source
TMP_DIR = tmp

CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(TMP_DIR)/%.o,$(CPP_FILES))

bin/citrus: $(OBJ_FILES)
	$(CC) -o bin/citrus $(CPP_FILES)

$(TMP_DIR)/%.o: $(CPP_FILES)
	$(CC) -o $@ $< $(CCPARAM)

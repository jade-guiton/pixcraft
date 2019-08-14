SRC_DIR   := src
OBJ_DIR   := obj
GLSL_DIR  := glsl

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

LDFLAGS   := -lglfw3 -lopengl32 -lgdi32 -lfreetype -lharfbuzz
CPPFLAGS  := 
CXXFLAGS  := -MD -MP -std=c++11 -Wall -Wno-unused -IC:/lib/glad-core3.3/include -IC:/lib/stb -IC:/lib \
	-IC:/msys64/mingw64/include/freetype2 -IC:/msys64/mingw64/include/harfbuzz

runRelease: test.exe
	./test.exe

runDebug: test_debug.exe
	./test_debug.exe

clean:
	rm -f test.exe
	rm -f test_debug.exe
	rm -fr  obj/*
	mkdir obj/world

test.exe: CXXFLAGS := -O3 $(CXXFLAGS)
test_debug.exe: CXXFLAGS := -g $(CXXFLAGS)

test.exe test_debug.exe: $(OBJ_FILES)
	g++ -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(SRC_DIR)/shaders_src.cpp: include_shader_code.py $(GLSL_DIR)/*
	python include_shader_code.py

-include $(OBJ_FILES:.o=.d)

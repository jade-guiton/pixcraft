SRC_DIR   := src
OBJ_DIR   := obj
GLSL_DIR  := glsl

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

LDFLAGS   := -static -lglfw3 -lopengl32 \
	-lfreetype -lharfbuzz -lfreetype -lpng16 -lz -lbz2 -lgraphite2 -lgdi32 -lrpcrt4
CPPFLAGS  := 
CXXFLAGS  := -MD -MP -std=c++11 -Wall -Wno-unused \
	-Ilib -IC:/lib/utf8-cpp-2.3.4 -IC:/msys64/mingw64/include/freetype2 -IC:/msys64/mingw64/include/harfbuzz

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

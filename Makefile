SRC_DIR   := src
OBJ_DIR   := obj
GLSL_DIR  := glsl

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp) $(SRC_DIR)/shaders_src.cpp
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

LDFLAGS   := -LC:/lib/glfw-3.3-mingw-w64/lib -lglfw3 -lopengl32 -lgdi32
CPPFLAGS  := 
CXXFLAGS  := -O3 -std=c++11 -Wall -Wno-unused -IC:/lib/glfw-3.3-mingw-w64/include -IC:/lib/glad-core3.3/include -IC:/lib/glm-0.9.9.2 -IC:/lib/stb -IC:/lib

CXXFLAGS += -MMD

run: test.exe
	./test.exe

clean:
	rm -f test.exe
	rm -f $(OBJ_DIR)/*

test.exe: $(OBJ_FILES)
	g++ -o $@ $^  $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(SRC_DIR)/shaders_src.cpp: include_shader_code.py $(GLSL_DIR)/*
	python include_shader_code.py

-include $(OBJ_FILES:.o=.d)

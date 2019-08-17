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

run: release
	./test.exe

clean:
	rm -f test.exe
	rm -fr  obj/*
	mkdir obj/world

release: CXXFLAGS := -O3 $(CXXFLAGS)
release: test.exe
debug: CXXFLAGS := -g $(CXXFLAGS)
debug: test.exe
profiling: CXXFLAGS := -O3 -pg $(CXXFLAGS)
profiling: LDFLAGS := -pg $(LDFLAGS)
profiling: test.exe

test.exe: $(OBJ_FILES)
	g++ -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(SRC_DIR)/shaders_src.cpp: include_shader_code.py $(GLSL_DIR)/*
	python include_shader_code.py

-include $(OBJ_FILES:.o=.d)

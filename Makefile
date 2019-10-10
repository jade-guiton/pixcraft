SRC_DIR   := src
OBJ_DIR   := obj
GLSL_DIR  := glsl

SRC_FILES := $(wildcard $(SRC_DIR)/*/*/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

LDFLAGS   := -static -lglfw3 -lopengl32 \
	-lfreetype -lharfbuzz -lfreetype -lpng16 -lz -lbz2 -lgraphite2 -lgdi32 -lrpcrt4
CPPFLAGS  := 
CXXFLAGS  := -MD -MP -std=c++17 -Wall -Wno-unused \
	-Isrc -Ilib -IC:/lib/utf8-cpp-2.3.4 -IC:/msys64/mingw64/include/freetype2 -IC:/msys64/mingw64/include/harfbuzz

run: release
	./pixcraft.exe

clean:
	rm -f pixcraft.exe
	rm -fr  obj/*
	mkdir obj/pixcraft
	mkdir obj/pixcraft/server
	mkdir obj/pixcraft/client
	mkdir obj/pixcraft/util

release: CXXFLAGS := -O3 $(CXXFLAGS)
release: pixcraft.exe
debug: CXXFLAGS := -g $(CXXFLAGS)
debug: pixcraft.exe
profiling: CXXFLAGS := -O3 -pg $(CXXFLAGS)
profiling: LDFLAGS := -pg $(LDFLAGS)
profiling: pixcraft.exe

pixcraft.exe: getCommitHash $(OBJ_FILES)
	g++ -o $@ $(OBJ_FILES) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(SRC_DIR)/pixcraft/client/shaders_src.cpp: include_shader_code.py $(GLSL_DIR)/*
	python include_shader_code.py

getCommitHash: get_commit_hash.py
	python get_commit_hash.py

$(SRC_DIR)/pixcraft/util/serializer_generated.h: serializer.fbs
	flatc -c -o $(SRC_DIR) serializer.fbs

-include $(OBJ_FILES:.o=.d)

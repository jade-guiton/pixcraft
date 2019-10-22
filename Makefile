# Library compilation flags (change based on OS / install folders)
UTF8_CPP_C_FLAGS  := -Ilib/utf8-cpp-2.3.4
FREETYPE2_C_FLAGS := -IC:/msys64/mingw64/include/freetype2 -IC:/msys64/mingw64/include/harfbuzz

# Library linking flags (change based on OS)
GLFW_LD_FLAGS     := -lglfw3 -lopengl32
FREETYPE_LD_FLAGS := -lfreetype -lharfbuzz -lfreetype -lpng16 -lz -lbz2 -lgraphite2 -lusp10 -lgdi32 -lrpcrt4
OTHER_LD_FLAGS    := -static

PYTHON3 := python
OUTPUT := pixcraft.exe


# # LINUX FLAGS (VERY EXPERIMENTAL):
# # Library compilation flags (change based on OS / install folders)
# UTF8_CPP_C_FLAGS  := -Ilib/utf8-cpp-2.3.4
# FREETYPE2_C_FLAGS := -I/usr/include/freetype2 -I/usr/include/libpng16
# 
# # Library linking flags (change based on OS)
# GLFW_LD_FLAGS     := -lglfw -lGL -lX11 -lpthread -lXrandr
# FREETYPE_LD_FLAGS := -lfreetype -lharfbuzz -lfreetype -lpng16 -ldl -lm -pthread
# OTHER_LD_FLAGS    := 
# 
# PYTHON3 := python3
# OUTPUT := pixcraft


SRC_DIR   := src
OBJ_DIR   := obj
LIB_DIR   := lib
GLSL_DIR  := glsl

SERIALIZER_DIR := $(SRC_DIR)/pixcraft/util
COMMIT_HASH := $(SRC_DIR)/pixcraft/util/commit_hash.cpp
SHADERS_SRC := $(SRC_DIR)/pixcraft/client/shaders_src.cpp
SERIALIZER_GENERATED := $(SERIALIZER_DIR)/serializer_generated.h

SRC_FILES := $(wildcard $(SRC_DIR)/*/*/*.cpp) $(SHADERS_SRC) $(COMMIT_HASH)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

CPPFLAGS  := 
CXXFLAGS  := -MD -MP -std=c++17 -Wall -Wno-unused \
	-I$(SRC_DIR) -I$(LIB_DIR) $(UTF8_CPP_C_FLAGS) $(FREETYPE2_C_FLAGS)
LDFLAGS   := $(OTHER_LD_FLAGS) $(GLFW_LD_FLAGS) $(FREETYPE_LD_FLAGS)

run: release
	./$(OUTPUT)

clean:
	rm -f $(OUTPUT)
	rm -rf $(OBJ_DIR)
	rm -f $(COMMIT_HASH)
	rm -f $(SERIALIZER_GENERATED)
	rm -f $(SHADERS_SRC)
	mkdir $(OBJ_DIR)
	mkdir $(OBJ_DIR)/pixcraft
	mkdir $(OBJ_DIR)/pixcraft/server
	mkdir $(OBJ_DIR)/pixcraft/client
	mkdir $(OBJ_DIR)/pixcraft/util

release: CXXFLAGS := -O3 $(CXXFLAGS)
release: $(OUTPUT)
debug: CXXFLAGS := -g $(CXXFLAGS)
debug: $(OUTPUT)
profiling: CXXFLAGS := -O3 -pg $(CXXFLAGS)
profiling: LDFLAGS := -pg $(LDFLAGS)
profiling: $(OUTPUT)

$(OUTPUT): getCommitHash $(SERIALIZER_GENERATED) $(OBJ_FILES) buildExec

buildExec: $(OBJ_FILES)
	g++ -o $(OUTPUT) $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

getCommitHash: get_commit_hash.py
	$(PYTHON3) get_commit_hash.py

$(SHADERS_SRC): include_shader_code.py $(GLSL_DIR)/*
	$(PYTHON3) include_shader_code.py

$(SERIALIZER_GENERATED): serializer.fbs
	flatc -c -o $(SERIALIZER_DIR) serializer.fbs

-include $(OBJ_FILES:.o=.d)

#pragma once

#include <iostream>

#include "glfw.hpp"

extern const char* blockVertexShaderSource;
extern const char* blockFragmentShaderSource;

extern const char* cursorVertexShaderSource;
extern const char* cursorFragmentShaderSource;

extern const char* hotbarVertexShaderSource;
extern const char* hotbarFragmentShaderSource;

GlId loadBlockShaders();
GlId loadCursorShaders();
GlId loadHotbarShaders();

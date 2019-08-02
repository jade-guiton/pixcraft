#pragma once

#include <iostream>

#include "glfw.hpp"

extern const char* blockVertexShaderSource;
extern const char* blockFragmentShaderSource;

extern const char* cursorVertexShaderSource;
extern const char* cursorFragmentShaderSource;

GlId loadBlockShaders();
GlId loadCursorShaders();

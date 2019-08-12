#pragma once

#include "glfw.hpp"

extern const char* blockVertexShaderSource;
extern const char* blockFragmentShaderSource;

extern const char* pixelVertexShaderSource;
extern const char* identityVertexShaderSource;
extern const char* colorFragmentShaderSource;

GlId loadBlockProgram();
GlId loadCursorProgram();
GlId loadWaterOverlayProgram();

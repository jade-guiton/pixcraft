#pragma once

#include "glfw.hpp"

extern const char* blockVertexShaderSource;
extern const char* blockFragmentShaderSource;
extern const char* blockGeometryShaderSource;

extern const char* cursorVertexShaderSource;
extern const char* overlayVertexShaderSource;
extern const char* colorFragmentShaderSource;

extern const char* entityVertexShaderSource;
extern const char* entityFragmentShaderSource;

extern const char* textVertexShaderSource;
extern const char* textFragmentShaderSource;

extern const char* blockOverlayVertexShaderSource;

GlId loadBlockProgram();
GlId loadCursorProgram();
GlId loadColorOverlayProgram();
GlId loadEntityProgram();
GlId loadTextProgram();
GlId loadBlockOverlayProgram();

#pragma once

// This file should be used instead of including GLFW or GLAD directly.
// This is to make sure that they included in the correct order.

#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef uint32_t GlId;

void checkGlErrors(const char* opDesc);

struct RenderParams {
	float skyColor[3];
	
	bool applyView;
	bool applyFog;
	
	float fogStart;
	float fogEnd;
};

#version 330 core

uniform vec2 winSize;

layout (location = 0) in vec2 attrPos;
layout (location = 1) in vec2 attrUV;

out vec2 vertUV;

void main() {
	vec2 scrPos = attrPos / winSize * 2;
	gl_Position = vec4(scrPos, 0.0, 1.0);
	vertUV = attrUV;
}

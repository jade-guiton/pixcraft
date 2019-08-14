#version 330 core

uniform vec2 winSize;

layout (location = 0) in vec2 attrPos;

void main() {
	vec2 scrPos = attrPos / winSize * 2;
	gl_Position = vec4(scrPos, 0.0, 1.0);
}

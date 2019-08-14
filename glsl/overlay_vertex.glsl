#version 330 core

layout (location = 0) in vec2 attrPos;

void main() {
	gl_Position = vec4(attrPos, 0.0, 1.0);
}

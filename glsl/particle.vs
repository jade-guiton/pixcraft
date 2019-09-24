#version 330 core

uniform mat4 view;
uniform mat4 proj;

uniform float fovY;
uniform float winH;

layout(location = 0) in vec3 attrPos;
layout(location = 1) in vec4 attrColor;
layout(location = 2) in float attrSize;

out vec4 partColor;

void main() {
	vec4 cameraSpace = view * vec4(attrPos, 1.0);
	gl_Position = proj * cameraSpace;
	gl_PointSize = atan(attrSize/2 / (-cameraSpace.z))/fovY*winH;
	partColor = attrColor;
}

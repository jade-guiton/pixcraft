#version 330 core

uniform mat4 model;
uniform bool applyView;
uniform mat4 view;
uniform mat4 proj;

layout (location = 0) in vec3 attrPos;
layout (location = 1) in vec2 attrUV;

out vec3 cameraCoords;
out vec2 vertexUV;

void main() {
	vec4 cameraSpace = model * vec4(attrPos, 1.0);
	if(applyView) cameraSpace = view * cameraSpace;
	cameraCoords = cameraSpace.xyz;
	gl_Position = proj * cameraSpace;
	vertexUV = attrUV;
}

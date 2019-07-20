#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 sideTransforms[6];

layout (location = 0) in vec3 attrPos;
layout (location = 1) in vec2 attrUV;
layout (location = 2) in vec3 instOffset;
layout (location = 3) in int instSide;
layout (location = 4) in int instTexId;

out vec3 cameraCoords;
out vec2 vertexUV;
out flat int faceTexId;
out vec3 normal;

void main() {
	mat4 sideTransform = sideTransforms[instSide];
	vec4 cameraSpace = view * model * (vec4(instOffset, 0.0) + sideTransform * vec4(attrPos.x, attrPos.y, attrPos.z, 1.0));
	cameraCoords = cameraSpace.xyz;
	gl_Position = proj * cameraSpace;
	vertexUV = attrUV;
	faceTexId = instTexId;
	normal = vec3(sideTransform * vec4(0.0, 0.0, 1.0, 1.0));
}

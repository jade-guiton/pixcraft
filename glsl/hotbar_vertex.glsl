#version 330 core

uniform float scale;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

layout (location = 0) in vec3 attrPos;
layout (location = 1) in vec2 attrUV;
layout (location = 2) in vec3 attrNormal;
layout (location = 3) in int attrTexId;

out vec2 vertexUV;
out vec3 normal;
out flat int faceTexId;

void main() {
	gl_Position = proj * view * model * vec4(scale * attrPos, 1.0);
	vertexUV = attrUV;
	normal = mat3(view * model) * attrNormal;
	faceTexId = attrTexId;
}

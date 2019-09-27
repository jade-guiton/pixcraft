#version 330 core

uniform mat4 view;
uniform mat4 proj;

uniform float fovY;
uniform float winH;

layout(location = 0) in vec3 attrPos;
layout(location = 1) in float attrSize;
layout(location = 2) in int attrTexId;
layout(location = 3) in vec2 attrTexCoord;

out float size;
flat out int texId;
out vec2 texCoord;

void main() {
	vec4 cameraSpace = view * vec4(attrPos, 1.0);
	gl_Position = proj * cameraSpace;
	gl_PointSize = atan(attrSize/2 / (-cameraSpace.z))/fovY*winH;
	size = attrSize;
	texId = attrTexId;
	texCoord = attrTexCoord;
}

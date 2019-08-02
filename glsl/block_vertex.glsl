#version 330 core

uniform mat4 model;
uniform bool applyView;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 sideTransforms[6];

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
	mat3 sideTransform = sideTransforms[instSide];
	vec4 cameraSpace = model * vec4(instOffset + sideTransform * attrPos, 1.0);
	if(applyView) cameraSpace = view * cameraSpace;
	cameraCoords = cameraSpace.xyz;
	gl_Position = proj * cameraSpace;
	vertexUV = attrUV;
	faceTexId = instTexId;
	normal = normalize(mat3(model) * sideTransform * vec3(0, 0, 1));
	if(!applyView) normal = mat3(inverse(view)) * normal;
}

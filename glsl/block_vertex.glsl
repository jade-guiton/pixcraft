#version 330 core

uniform mat4 model;
uniform bool applyView;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 sideTransforms[6];

layout(location = 0) in vec3 attrPos;
layout(location = 1) in vec2 attrUV;
layout(location = 2) in vec3 instOffset;
layout(location = 3) in int instSide;
layout(location = 4) in int instTexId;

out VS_OUT {
	vec3 cameraCoords;
	vec2 vertexUV;
	flat int faceTexId;
	vec3 normal;
} vs_out;

void main() {
	mat3 sideTransform = sideTransforms[instSide];
	vec4 cameraSpace = model * vec4(instOffset + sideTransform * attrPos, 1.0);
	if(applyView) cameraSpace = view * cameraSpace;
	vs_out.cameraCoords = cameraSpace.xyz;
	gl_Position = proj * cameraSpace;
	vs_out.vertexUV = attrUV;
	vs_out.faceTexId = instTexId;
	vs_out.normal = normalize(mat3(model) * sideTransform * vec3(0, 0, 1));
	if(!applyView) vs_out.normal = mat3(inverse(view)) * vs_out.normal;
}

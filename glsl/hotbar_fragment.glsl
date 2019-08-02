#version 330 core

uniform sampler2DArray texArray;

uniform float ambientLight;
uniform float diffuseLight;
uniform vec3 lightSrcDir;

in vec2 vertexUV;
in vec3 normal;
in flat int faceTexId;

out vec4 fragColor;

void main() {
	fragColor = texture(texArray, vec3(vertexUV, faceTexId));
	
	float light = ambientLight + diffuseLight*max(dot(lightSrcDir, normal), 0);
	fragColor.rgb *= min(light, 1);
} 
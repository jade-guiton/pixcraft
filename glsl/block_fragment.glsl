#version 330 core

uniform sampler2DArray texArray;

uniform float ambientLight;
uniform float diffuseLight;
uniform vec3 lightSrcDir;

uniform bool applyFog;
uniform vec4 fogColor;
uniform float fogStart;
uniform float fogEnd;

in GS_OUT {
	vec3 cameraCoords;
	vec2 vertexUV;
	flat int faceTexId;
	vec3 normal;
} fs_in;

out vec4 fragColor;

void main() {
	fragColor = texture(texArray, vec3(fs_in.vertexUV, fs_in.faceTexId));
	
	if(fragColor.a == 0) discard;
	
	float light = ambientLight + diffuseLight*max(dot(lightSrcDir, fs_in.normal), 0);
	fragColor.rgb *= min(light, 1);
	
	if(applyFog) {
		float dist = length(fs_in.cameraCoords);
		if(dist > fogEnd) discard;
		float fogFactor = clamp((fogEnd - dist)/(fogEnd - fogStart), 0.0, 1.0);
		fragColor = mix(fogColor, fragColor, fogFactor);
	}
} 
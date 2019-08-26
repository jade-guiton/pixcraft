#version 330 core

uniform sampler2D tex;

uniform float ambientLight;

uniform bool applyFog;
uniform vec4 fogColor;
uniform float fogStart;
uniform float fogEnd;

in vec3 cameraCoords;
in vec2 vertexUV;

out vec4 fragColor;

void main() {
	fragColor = texture(tex, vertexUV);
	
	if(fragColor.a == 0) discard;
	
	float light = ambientLight;
	fragColor.rgb *= min(light, 1);
	
	if(applyFog) {
		float dist = length(cameraCoords);
		if(dist > fogEnd) discard;
		float fogFactor = clamp((fogEnd - dist)/(fogEnd - fogStart), 0.0, 1.0);
		fragColor = mix(fogColor, fragColor, fogFactor);
	}
} 
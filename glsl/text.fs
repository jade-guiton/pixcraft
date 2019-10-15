#version 330 core

uniform sampler2D tex;
uniform vec4 textColor;

in vec2 vertUV;

out vec4 fragColor;

void main() {
	vec3 outlineColor = 1 - textColor.rgb;
	float outlineSample = texture(tex, vertUV).r;
	float sample = texture(tex, vertUV).g;
	float outlineFactor = outlineSample*(1 - sample);
	float alpha = sample + outlineFactor;
	vec3 color = (textColor.rgb*sample + outlineColor*outlineFactor) / alpha;
	fragColor = vec4(color, alpha * textColor.a);
}

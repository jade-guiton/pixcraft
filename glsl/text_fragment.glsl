#version 330 core

uniform sampler2D tex;
uniform vec3 textColor;

in vec2 vertUV;

out vec4 fragColor;

void main() {    
	//fragColor = vec4(textColor, texture(tex, vertUV).r);
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, vertUV).r);
	fragColor = vec4(textColor, 1.0) * sampled;
}

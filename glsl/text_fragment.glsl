#version 330 core

uniform sampler2D tex;
uniform vec3 textColor;

in vec2 vertUV;

out vec4 fragColor;

void main() {
	fragColor = vec4(textColor, texture(tex, vertUV).r);
}

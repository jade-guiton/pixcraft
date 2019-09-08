#version 330 core

uniform sampler2D tex;

in vec2 vertUV;

out vec4 fragColor;

void main() {
	fragColor = texture(tex, vertUV);
}

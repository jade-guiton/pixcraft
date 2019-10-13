#version 330 core

uniform sampler2D tex;

in vec2 vertUV;

out vec4 fragColor;

void main() {
	fragColor = texture(tex, vertUV);
	if(vertUV.x < 0 || vertUV.x > 1 || vertUV.y < 0 || vertUV.y > 1)
		fragColor = vec4(1, 0, 0, 1);
}

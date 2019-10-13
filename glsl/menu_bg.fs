#version 330 core

uniform sampler2DArray texArray;
uniform int texId;

uniform vec2 winSize;
uniform float tileSize;
uniform vec2 offset;

in vec2 vertPos;

out vec4 fragColor;

void main() {
	fragColor = texture(texArray, vec3(mod(vertPos*winSize/2/tileSize + offset, 1), texId));
}

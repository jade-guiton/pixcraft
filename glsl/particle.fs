#version 330 core

uniform sampler2DArray texArray;
uniform float blockTexSize;

in float size;
flat in int texId;
in vec2 texCoord;

out vec4 fragColor;

void main() {
	fragColor = texture(texArray, vec3(texCoord + gl_PointCoord*size/blockTexSize, texId));
	if(fragColor.a == 0) discard;
}
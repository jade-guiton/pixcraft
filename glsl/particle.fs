#version 330 core

uniform sampler2DArray texArray;

flat in int texId;
in vec2 texCoord;

out vec4 fragColor;

void main() {
	fragColor = texture(texArray, vec3(texCoord + gl_PointCoord/16, texId));
	if(fragColor.a == 0) discard;
}
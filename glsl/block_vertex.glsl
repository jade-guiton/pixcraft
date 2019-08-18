#version 330 core

layout(location = 0) in vec3 attrPos;
layout(location = 1) in int attrSide;
layout(location = 2) in int attrTexId;

out VS_OUT {
	int side;
	int texId;
} vs_out;

void main() {;
	gl_Position = vec4(attrPos, 1.0);
	vs_out.side = attrSide;
	vs_out.texId = attrTexId;
}

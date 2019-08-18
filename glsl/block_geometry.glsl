#version 330 core

layout(triangles) in;

in VS_OUT {
	vec3 cameraCoords;
	vec2 vertexUV;
	flat int faceTexId;
	vec3 normal;
} gs_in[];

layout(triangle_strip, max_vertices = 3) out;

out GS_OUT {
	vec3 cameraCoords;
	vec2 vertexUV;
	flat int faceTexId;
	vec3 normal;
} gs_out;

void main() {
	for(int i = 0; i < 3; i++) {
		gl_Position = gl_in[i].gl_Position;
		gs_out.cameraCoords = gs_in[i].cameraCoords;
		gs_out.vertexUV = gs_in[i].vertexUV;
		gs_out.faceTexId = gs_in[i].faceTexId;
		gs_out.normal = gs_in[i].normal;
		EmitVertex();
	}
	EndPrimitive();
}
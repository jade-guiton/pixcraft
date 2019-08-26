#version 330 core

#pragma optionNV (unroll all)

uniform mat4 model;
uniform bool applyView;
uniform mat4 view;

uniform mat4 proj;
uniform mat3 sideTransforms[6];

layout(points) in;

in VS_OUT {
	int side;
	int texId;
} gs_in[];

layout(triangle_strip, max_vertices = 4) out;

out GS_OUT {
	flat int texId;
	flat vec3 normal;
	vec3 cameraCoords;
	vec2 vertexUV;
} gs_out;

void main() {
	mat3 sideTransform = sideTransforms[gs_in[0].side];
	
	gs_out.texId = gs_in[0].texId;
	gs_out.normal = normalize(mat3(model) * sideTransform * vec3(0, 0, 1));
	if(!applyView) gs_out.normal = mat3(inverse(view)) * gs_out.normal;
	
	for(int y = 0; y <= 1; y++) {
		for(int x = 0; x <= 1; x++) {
			vec4 cameraCoords = model * (gl_in[0].gl_Position + vec4(sideTransform * vec3(x - 0.5, y - 0.5, 0.5), 0.0));
			if(applyView)
				cameraCoords = view * cameraCoords;
			gs_out.cameraCoords = vec3(cameraCoords);
			gl_Position = proj * cameraCoords;
			gs_out.vertexUV = vec2(x, y);
			EmitVertex();
		}
	}
	EndPrimitive();
}
#include "shaders.hpp"

namespace PixCraft::ShaderSources {

const char* blockVS = R"glsl(
#version 330 core

layout(location = 0) in uvec3 attrPos;
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
)glsl";

const char* blockGS = R"glsl(
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
})glsl";

const char* blockFS = R"glsl(
#version 330 core

uniform sampler2DArray texArray;

uniform float ambientLight;
uniform float diffuseLight;
uniform vec3 lightSrcDir;

uniform bool applyFog;
uniform vec4 fogColor;
uniform float fogStart;
uniform float fogEnd;

in GS_OUT {
	flat int texId;
	flat vec3 normal;
	vec3 cameraCoords;
	vec2 vertexUV;
} fs_in;

out vec4 fragColor;

void main() {
	fragColor = texture(texArray, vec3(fs_in.vertexUV, fs_in.texId));
	
	if(fragColor.a == 0) discard;
	
	float light = ambientLight + diffuseLight*max(dot(lightSrcDir, fs_in.normal), 0);
	fragColor.rgb *= min(light, 1);
	
	if(applyFog) {
		float dist = length(fs_in.cameraCoords);
		if(dist > fogEnd) discard;
		float fogFactor = clamp((fogEnd - dist)/(fogEnd - fogStart), 0.0, 1.0);
		fragColor = mix(fogColor, fragColor, fogFactor);
	}
} )glsl";

const char* entityVS = R"glsl(
#version 330 core

uniform mat4 model;
uniform bool applyView;
uniform mat4 view;
uniform mat4 proj;

layout (location = 0) in vec3 attrPos;
layout (location = 1) in vec2 attrUV;

out vec3 cameraCoords;
out vec2 vertexUV;

void main() {
	vec4 cameraSpace = model * vec4(attrPos, 1.0);
	if(applyView) cameraSpace = view * cameraSpace;
	cameraCoords = cameraSpace.xyz;
	gl_Position = proj * cameraSpace;
	vertexUV = attrUV;
}
)glsl";

const char* entityFS = R"glsl(
#version 330 core

uniform sampler2D tex;

uniform float ambientLight;

uniform bool applyFog;
uniform vec4 fogColor;
uniform float fogStart;
uniform float fogEnd;

in vec3 cameraCoords;
in vec2 vertexUV;

out vec4 fragColor;

void main() {
	fragColor = texture(tex, vertexUV);
	
	if(fragColor.a == 0) discard;
	
	float light = ambientLight;
	fragColor.rgb *= min(light, 1);
	
	if(applyFog) {
		float dist = length(cameraCoords);
		if(dist > fogEnd) discard;
		float fogFactor = clamp((fogEnd - dist)/(fogEnd - fogStart), 0.0, 1.0);
		fragColor = mix(fogColor, fragColor, fogFactor);
	}
} )glsl";

const char* textVS = R"glsl(
#version 330 core

uniform vec2 winSize;

layout (location = 0) in vec2 attrPos;
layout (location = 1) in vec2 attrUV;

out vec2 vertUV;

void main() {
	vec2 screenPos = attrPos / winSize * 2 - 1;
	screenPos.y = -screenPos.y;
	gl_Position = vec4(screenPos, 0.0, 1.0);
	vertUV = attrUV;
}
)glsl";

const char* textFS = R"glsl(
#version 330 core

uniform sampler2D tex;
uniform vec3 textColor;

in vec2 vertUV;

out vec4 fragColor;

void main() {
	vec3 outlineColor = 1 - textColor;
	float outlineSample = texture(tex, vertUV).r;
	float sample = texture(tex, vertUV).g;
	float outlineFactor = outlineSample*(1 - sample);
	float alpha = sample + outlineFactor;
	vec3 color = (textColor*sample + outlineColor*outlineFactor) / alpha;
	fragColor = vec4(color, alpha);
}
)glsl";

const char* cursorVS = R"glsl(
#version 330 core

uniform vec2 winSize;

layout (location = 0) in vec2 attrPos;

void main() {
	vec2 scrPos = attrPos / winSize * 2;
	gl_Position = vec4(scrPos, 0.0, 1.0);
}
)glsl";

const char* overlayVS = R"glsl(
#version 330 core

layout (location = 0) in vec2 attrPos;

void main() {
	gl_Position = vec4(attrPos, 0.0, 1.0);
}
)glsl";

const char* blockOverlayVS = R"glsl(
#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

layout (location = 0) in vec3 attrPos;

void main() {
	gl_Position = proj * view * model * vec4(attrPos, 1.0);
}
)glsl";

const char* buttonVS = R"glsl(
#version 330 core

uniform vec2 winSize;

layout (location = 0) in vec2 attrPos;
layout (location = 1) in vec2 attrUV;

out vec2 vertUV;

void main() {
	vec2 scrPos = attrPos / winSize * 2;
	gl_Position = vec4(scrPos, 0.0, 1.0);
	vertUV = attrUV;
}
)glsl";

const char* colorFS = R"glsl(
#version 330 core

uniform vec4 color;

out vec4 fragColor;

void main() {
    fragColor = color;
})glsl";

const char* textureFS = R"glsl(
#version 330 core

uniform sampler2D tex;

in vec2 vertUV;

out vec4 fragColor;

void main() {
	fragColor = texture(tex, vertUV);
}
)glsl";

}

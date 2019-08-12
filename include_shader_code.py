with open("src/shaders_src.cpp", "w") as out:
	out.write("#include \"shaders.hpp\"\n\n")
	
	def include(path, shaderName):
		with open(path) as f:
			code = f.read()
			out.write("const char* {}ShaderSource = R\"glsl(\n".format(shaderName))
			out.write(code)
			out.write(")glsl\";\n\n")
	
	include("glsl/block_vertex.glsl", "blockVertex")
	include("glsl/block_fragment.glsl", "blockFragment")
	
	include("glsl/pixel_vertex.glsl", "pixelVertex")
	include("glsl/identity_vertex.glsl", "identityVertex")
	include("glsl/color_fragment.glsl", "colorFragment")
	
	include("glsl/entity_vertex.glsl", "entityVertex")
	include("glsl/entity_fragment.glsl", "entityFragment")

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
	
	include("glsl/cursor_vertex.glsl", "cursorVertex")
	include("glsl/cursor_fragment.glsl", "cursorFragment")
	
	include("glsl/hotbar_vertex.glsl", "hotbarVertex")
	include("glsl/hotbar_fragment.glsl", "hotbarFragment")

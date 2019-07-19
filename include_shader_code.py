with open("src/shaders_src.cpp", "w") as out:
	out.write("#include \"shaders.hpp\"\n\n")
	
	with open("glsl/vertex.glsl") as f:
		vertex_code = f.read()
		out.write("const char* vertexShaderSource = R\"glsl(\n")
		out.write(vertex_code)
		out.write(")glsl\";\n")
	
	out.write("\n")
	
	with open("glsl/fragment.glsl") as f:
		fragment_code = f.read()
		out.write("const char* fragmentShaderSource = R\"glsl(\n")
		out.write(fragment_code)
		out.write(")glsl\";\n")

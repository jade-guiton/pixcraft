with open("src/shaders_src.cpp", "w") as out:
	out.write("#include \"shaders.hpp\"\n\n")
	
	def include(path, shaderName):
		with open(path) as f:
			code = f.read()
			out.write("const char* {}ShaderSource = R\"glsl(\n".format(shaderName))
			out.write(code)
			out.write(")glsl\";\n\n")
	
	include("glsl/block_vertex.glsl", "blockVertex")
	include("glsl/block_geometry.glsl", "blockGeometry")
	include("glsl/block_fragment.glsl", "blockFragment")
	
	include("glsl/cursor_vertex.glsl", "cursorVertex")
	include("glsl/overlay_vertex.glsl", "overlayVertex")
	include("glsl/color_fragment.glsl", "colorFragment")
	
	include("glsl/entity_vertex.glsl", "entityVertex")
	include("glsl/entity_fragment.glsl", "entityFragment")
	
	include("glsl/text_vertex.glsl", "textVertex")
	include("glsl/text_fragment.glsl", "textFragment")
	
	include("glsl/block_overlay_vertex.glsl", "blockOverlayVertex")

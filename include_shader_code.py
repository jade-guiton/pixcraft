with open("src/pixcraft/client/shaders_src.cpp", "w") as out:
	out.write("#include \"shaders.hpp\"\n\n")
	out.write("namespace PixCraft::ShaderSources {\n\n")
	
	def include(path, shaderName):
		with open(path) as f:
			code = f.read()
			out.write("const char* {} = R\"glsl(\n".format(shaderName))
			out.write(code)
			out.write(")glsl\";\n\n")
	
	include("glsl/block.vs", "blockVS")
	include("glsl/block.gs", "blockGS")
	include("glsl/block.fs", "blockFS")
	
	include("glsl/entity.vs", "entityVS")
	include("glsl/entity.fs", "entityFS")
	
	include("glsl/particle.vs", "particleVS")
	include("glsl/particle.fs", "particleFS")
	
	include("glsl/menu_bg.vs", "menuBgVS")
	include("glsl/menu_bg.fs", "menuBgFS")
	
	include("glsl/cursor.vs", "cursorVS")
	include("glsl/overlay.vs", "overlayVS")
	include("glsl/block_overlay.vs", "blockOverlayVS")
	include("glsl/gui.vs", "guiVS")
	include("glsl/text.fs", "textFS")
	include("glsl/color.fs", "colorFS")
	include("glsl/texture.fs", "textureFS")
	
	out.write("}\n")
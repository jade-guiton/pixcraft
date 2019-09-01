import subprocess

if __name__ == '__main__':
	commit_hash = subprocess.run(["git", "rev-parse", "HEAD"], stdout=subprocess.PIPE).stdout.decode("utf-8")[:6]
	with open("src/commit_hash.cpp", "w") as f:
		f.write('#include "version.hpp"\n\n')
		f.write('const char* PixCraft::COMMIT_HASH = "{}";\n'.format(commit_hash))

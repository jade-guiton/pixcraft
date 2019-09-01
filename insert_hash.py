import sys
import re
import subprocess

if __name__ == '__main__':
	smudge = len(sys.argv) == 2 and sys.argv[1] == "smudge"
	clean = len(sys.argv) == 2 and sys.argv[1] == "clean"
	if not (smudge or clean):
		print("Usage: python insert_hash.py [smudge|clean]")
		print("Most useful as a Git commit hook.")
		exit(1)
	code = sys.stdin.read()
	if smudge:
		commit_hash = subprocess.run(["git", "rev-parse", "HEAD"], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
		print(code.replace('COMMIT_HASH("dev")', 'COMMIT_HASH("{}")'.format(commit_hash)), end="")
	elif clean:
		print(re.sub(r'COMMIT_HASH("[0-9a-f]+")', 'COMMIT_HASH("dev")', code), end="")

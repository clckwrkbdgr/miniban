import os

ACCEPTED = ['.cpp', '.h']
REJECTED = ['.swp', 'moc_']
SKIPPED_DIRS = ['.git']
COMMENT_MARKER = '//#'
README_FILENAME = './bin/README'

def find_files():
	all_files = []
	for dirpath, dirnames, filenames in os.walk('.', topdown=True):
		for dirname in SKIPPED_DIRS:
			if dirname in dirnames:
				dirnames.remove(dirname)

		filenames = filter(lambda s: any([value in s for value in ACCEPTED]), filenames)
		filenames = filter(lambda s: not any([value in s for value in REJECTED]), filenames)
		all_files += [os.path.join(dirpath, filename) for filename in filenames]
	return all_files

def extract_comments():
	result = []
	for filename in all_files:
		with open(filename, 'r') as f:
			lines = [line.rstrip() for line in f.readlines() if COMMENT_MARKER in line]
			for line in lines:
				code, marker, comment = line.partition(COMMENT_MARKER)
				result.append(comment.strip())
	return result

if __name__ == "__main__":
	all_files = find_files()
	readme_lines = extract_comments()
	readme_lines = [line + '\n' for line in readme_lines]
	with open(README_FILENAME, 'w') as f:
		f.writelines(readme_lines)

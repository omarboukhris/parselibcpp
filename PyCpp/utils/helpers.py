
import distutils.dir_util as dir_util
import os.path
import pathlib

from typing import List, Tuple

class ArgParser:
	""" For parsing cmd line arg
	must be formatted like :
	* param
	* param=value
	Value must not contain any space for Parser to work
	or the white space should be escaped "\\ "
	"""

	def __init__(self, argv: List[str]):
		self.parsedargv = {}
		for arg in argv:
			s = arg.split("=")
			if len(s) == 1:
				self.parsedargv[s[0]] = True
			elif len(s) == 2:
				self.parsedargv[s[0]] = s[1]

	def get(self, key: str):
		key = ArgParser.decorate_key(key)
		if key in self.parsedargv.keys():
			return self.parsedargv[key]
		return False

	@staticmethod
	def decorate_key(key: str) -> str:
		if len(key) == 1:
			return "-{}".format(key)
		elif len(key) >= 2 and key[:2] != "--":
			return "--{}".format(key)
		else:
			return key


def show_help(exe: str = "") -> None:
	print("\nusage :\n\t{} \n\
\t\t--pname=projectname \n\
\t\t--ptype=(so|a|x) \n\
\t\t--glob=regex_to_glob_files\n\
\t\t--ext=h,cpp,impl,py,ctype\n\
\t\t--plibs=\"list,of,libs,sep,by,comma\" \n\
\t\t--path=\"regex/to/glob\"\n\
\t\t-v -h\n\n\
\t-v is for verbose and -h is for help\n\
\tIf help is active, program shows this messages and exit.\n\
\tExtensions (ext) separated by <,> should not contain spaces\n".format(exe))

def find_directory(atom, root) -> str:
	for path, dirs, files in os.walk(root):
		if atom in dirs:
			return os.path.join(path, atom)
	return ""

def get_project_folder(ppath: str, get_dir_func: callable) -> str:
	""" Checks if project folder is valid otherwise tries to look for it

	:param ppath: project folder
	:param get_dir_func: get_directory function to use folder prompt with GUI
	:return: path to project folder, empty string if nothing found
	"""
	if ppath and type(ppath) == str:
		if os.path.isdir(ppath):
			project_folder = ppath
		else:
			ppath_list = ppath.split("**/")
			if len(ppath_list) == 2:
				dir_name, root_dir = ppath_list[1], ppath_list[0]
				directory = find_directory(dir_name, root_dir)
				project_folder = directory if directory else get_dir_func()
			else:
				project_folder = get_dir_func()
	else:
		project_folder = get_dir_func()
	return project_folder

def check_parser_input_args(argparser: ArgParser) -> Tuple:
	""" Check if arguments contain help command
	if project path is correct
	if regex used for globing is valid
	if output extensions are valid

	:param argparser: argument parser object
	:return: globing regex
	"""
	if argparser.get("h") or argparser.get("help"):
		show_help()
		exit()

	# handles project path argument
	project_path, new_path = argparser.get("ppath"), ""
	if os.path.isdir(project_path):
		new_path = str(pathlib.Path(project_path)) + "_pxx"
	else:
		project_path = get_project_folder(project_path, get_dir_func=lambda: "")
		new_path = str(pathlib.Path(project_path)) + "_pxx"
		if not new_path:
			print("An error occured: {} is not a directory".format(argparser.get("ppath")))
			exit()
	dir_util.copy_tree(project_path, new_path)
	project_path = new_path

	# handle input extension
	regex_glob = argparser.get("glob")
	if not regex_glob or not isinstance(regex_glob, str):
		print("Wrong regex glob argument  <{}> using default .cxx".format(regex_glob))
		regex_glob = "*.cxx"

	# handle output extensions
	all_ext = ["cpp", "h", "impl", "py", "ctype"]
	out_ext = argparser.get("ext")
	if not out_ext or not isinstance(out_ext, str):
		out_ext = all_ext
	else:
		out_ext = out_ext.split(",")

	return project_path, regex_glob, out_ext

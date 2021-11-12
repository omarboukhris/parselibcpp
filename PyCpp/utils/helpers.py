
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
	def decorate_key(key: str):
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
	project_path = argparser.get("path")
	if os.path.isdir(project_path):
		new_path = str(pathlib.Path(project_path)) + "_pxx"
		dir_util.copy_tree(project_path, new_path)
		project_path = new_path
	else:
		print("An error occured: {} is not a directory".format(project_path))
		exit()

	# handle input extension
	regex_glob = argparser.get("glob")
	if not regex_glob or type(regex_glob) != str:
		print("Wrong regex glob argument  <{}> using default .cxx".format(regex_glob))
		regex_glob = "*.cxx"

	# handle output extensions
	all_ext = ["cpp", "h", "impl", "py", "ctype"]
	out_ext = argparser.get("ext")
	if not out_ext or type(out_ext) != str:
		out_ext = all_ext
	else:
		out_ext = out_ext.split(",")

	return project_path, regex_glob, out_ext

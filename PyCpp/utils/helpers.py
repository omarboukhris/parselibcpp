
import distutils.dir_util as dir_util
import os.path
import pathlib

class ArgParser:
	""" For parsing cmd line arg
	must be formatted like :
	* param
	* param=value
	Value must not contain any space for Parser to work
	"""

	def __init__(self, argv):
		self.parsedargv = {}
		for arg in argv:
			s = arg.split("=")
			if len(s) == 1:
				self.parsedargv[s[0]] = True
			elif len(s) == 2:
				self.parsedargv[s[0]] = s[1]

	def get(self, key):
		key = ArgParser.decorate_key(key)
		if key in self.parsedargv.keys():
			return self.parsedargv[key]
		return False

	@staticmethod
	def decorate_key(key):
		if len(key) == 1:
			return "-{}".format(key)
		elif len(key) >= 2 and key[:2] != "--":
			return "--{}".format(key)
		else:
			return key


def show_help(exe: str = ""):
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

def prepare_folder(ppath: str):
	if os.path.isdir(ppath):
		new_path = str(pathlib.Path(ppath)) + "_pxx"
		dir_util.copy_tree(ppath, new_path)
		return new_path
	else:
		raise NotADirectoryError(ppath + " is not a directory")

def check_arg(argparser: ArgParser):
	""" Check if arguments contain help command or
	if regex used for globing is valid

	:param argparser: argument parser object
	:return: globing regex
	"""
	if argparser.get("h") or argparser.get("help"):
		show_help()
		exit()

	project_path = argparser.get("path")

	try:
		project_path = prepare_folder(project_path)
	except NotADirectoryError as e:
		print("An exception occured: ", e)
		print("Exiting program")
		exit()

	regex_glob = argparser.get("glob")

	if not regex_glob or type(regex_glob) != str:
		print("Wrong argument : ", regex_glob)
		show_help()
		exit()

	all_ext = ["cpp", "h", "impl", "py", "ctype"]
	out_ext = all_ext if not argparser.get("ext") else argparser.get("ext").split(",")

	return project_path, regex_glob, out_ext




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

	# else :
	# pass
	def get(self, key):
		if key in self.parsedargv.keys():
			return self.parsedargv[key]
		return False


def show_help(exe: str = ""):
	print("\nusage :\n\t{} \n\
\t\tpname=projectname \n\
\t\tptype=(so|a|x) \n\
\t\tglob=regex_to_glob_files\n\
\t\text=h,cpp,impl,py,ctype\n\
\t\tplibs=\"list,of,libs,sep,by,comma\" \n\
\t\tfiles=\"regex/to/glob\"\n\
\t\tv h\n\n\
\tv is for verbose and h is for help\n\
\tIf help is active, program shows this messages and exit.\n\
\tExtensions (ext) separated by <,> should not contain spaces\n".format(exe))


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
	regex_glob = argparser.get("glob")

	if not regex_glob or type(regex_glob) != str:
		print("Wrong argument : ", regex_glob)
		show_help()
		exit()

	all_ext = ["cpp", "h", "impl", "py", "ctype"]
	out_ext = all_ext if not argparser.get("ext") else argparser.get("ext").split(",")

	return project_path, regex_glob, out_ext



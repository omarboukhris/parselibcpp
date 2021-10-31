
from PyCpp import pycppeng, cmakegen as cmk
from PyCpp.parsesession import ParseSession, ArgParser
from PyCpp.factory import PyCppFactory, FileNameProcessor

import sys
import glob

def show_help():
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
\tExtensions (ext) separated by <,> should not contain spaces\n".format(sys.argv[0]))

def check_arg(argparser: ArgParser):
	""" Check if arguments contain help command or
	if regex used for globing is valid

	:param argparser: argument parser object
	:return: globing regex
	"""
	if argparser.get("h") or argparser.get("help"):
		show_help()
		exit()

	regex_glob = argparser.get("glob")

	if not regex_glob or type(regex_glob) != str:
		print("Wrong argument : ", regex_glob)
		show_help()
		exit()

	all_ext = ["cpp", "h", "impl", "py", "ctype"]
	out_ext = all_ext if not argparser.get("ext") else argparser.get("ext").split(",")

	return regex_glob, out_ext

def main():
	argp = ArgParser(sys.argv)

	globex, output_ext = check_arg(argp)

	psess = ParseSession()
	psess.load_grammar("PyCpp/data/grammar.grm", False)

	processed_files = []
	for jfile in glob.glob(globex):

		# call parselib parser
		print("parselib > processing file \"{}\"".format(jfile))
		parsed_json = psess.parse_to_json(jfile, False)
		# print(parsed_json)

		# prepare streams and observers
		active_streams = PyCppFactory.fs_fabric(jfile, output_ext)
		observers = PyCppFactory.gen_fabric(output_ext, active_streams)

		# call main generator
		gen = pycppeng.PyCppEngine(parsed_json, observers)
		gen.drive()

		# output results
		if argp.get("v"):
			for ext, output in zip(output_ext, active_streams):
				print(ext, "-----------------------------\n")
				print(output)

		# activate to write output to file
		# for stream in active_streams:
		# 	stream.write()

		processed_files.append(jfile)

	del psess

	#
	# cmake generator here
	#
	pname = argp.get("pname")
	ptype = argp.get("ptype")
	plibs = argp.get("plibs").split(",") if argp.get("plibs") else []
	cpp_ver = argp.get("cpp") if argp.get("cpp") in ["11", "14", "17", "20"] else "17"
	cmk_ver = argp.get("cmk") if argp.get("cmk") else "3.5"
	dbg = argp.get("dbgflg") if argp.get("dbgflg") else "-g"
	rel = argp.get("relflg") if argp.get("relflg") else "-O2"

	# check for valid parameters
	assert type(pname) == str, \
		"Specify project name -> pname=the_name, pname value is <{}>".format(pname)
	assert ptype in ["so", "a", "x"], \
		"Specify project type -> ptype=(so|a|x), ptype value is <{}>".format(ptype)

	fnproc = FileNameProcessor(processed_files, output_ext)
	cmake = cmk.CMakeGenerator(
		argp.get("pname"),
		argp.get("ptype"),
		fnproc,
		plibs,
		cmk_ver,
		cpp_ver,
		dbg,
		rel
	)

	print(cmake.files.get_files())
	print(cmake.make_header())
	print(cmake.make_files())
	print(cmake.make_builder())

if __name__ == "__main__":
	main()

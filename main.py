
from PyCpp.parsesession import ParseSession, StringStream, ArgParser
from PyCpp.observers import HppGenerator, TemplGenerator, CppGenerator, GatewayGenerator, PyGwGenerator
from PyCpp import pycppeng, cmakegen as cmk

import sys
import glob

def ss_fabric(out_ext: list = []):
	out = []
	for ext in out_ext:
		if ext in ["cpp"]:
			out.append(StringStream())
		elif ext in ["h", "hpp"]:
			out.append(StringStream())
		elif ext in ["pyc"]:
			out.append(StringStream())
		elif ext in ["ctype"]:
			out.append(StringStream())
		elif ext in ["impl"]:
			out.append(StringStream())
	return out

def gen_fabric(out_ext: list = (), streams: list = ()):
	out = []
	for ext, stream in zip(out_ext, streams):
		if ext in ["cpp"]:
			out.append(CppGenerator(stream))
		elif ext in ["h", "hpp"]:
			out.append(HppGenerator(stream))
		elif ext in ["pyc"]:
			out.append(PyGwGenerator(stream))
		elif ext in ["ctype"]:
			out.append(GatewayGenerator(stream))
		elif ext in ["impl"]:
			out.append(TemplGenerator(stream))
	return out

def show_help():
	print("\nusage :\n\t{} \n\
\t\tpname=projectname \n\
\t\tptype=(so|a|x) \n\
\t\text=h,cpp,impl,py,ctype\n\
\t\tplibs=\"list,of,libs,sep,by,comma\" \n\
\t\tfiles=\"regex/to/glob\"\n\
\t\tv h\n\n\
\tv is for verbose and h is for help\n\
\tIf help is active, program shows this messages and exit.\n\
\tExtensions (ext) separated by <,> should not contain spaces\n".format(sys.argv[0]))


if __name__ == "__main__":
	argp = ArgParser(sys.argv)

	if argp.get("h") or argp.get("help"):
		show_help()
		exit()

	psess = ParseSession()
	psess.load_grammar("PyCpp/data/grammar.grm", False)

	processed_files = []
	for jfile in glob.glob("PyCpp/data/test_srcs/*.java"):

		# call parselib parser
		print("parselib > processing file \"{}\"".format(jfile))
		parsed_json = psess.parse_to_json(jfile, False)
		# print(parsed_json)

		# prepare streams and observers
		all_ext = ["cpp", "h", "impl", "py"]
		print(argp.get("ext"))
		output_ext = all_ext if not argp.get("ext") else argp.get("ext").split(",")
		str_streams = ss_fabric(output_ext)
		observers = gen_fabric(output_ext, str_streams)

		# call main generator
		gen = pycppeng.PyCppEngine(parsed_json, observers)
		gen.drive()

		# output results
		if argp.get("v"):
			for ext, output in zip(output_ext, str_streams):
				print(ext, "-----------------------------\n")
				print(output)

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

	cmake = cmk.CMakeGenerator(
		argp.get("pname"),
		argp.get("ptype"),
		processed_files,
		plibs,
		cmk_ver,
		cpp_ver
	)
	print(cmake.files)
	print(cmake.make_header())
	print(cmake.make_files())
	print(cmake.make_builder())

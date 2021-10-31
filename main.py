
from PyCpp import pycppeng, cmakegen as cmk
from PyCpp.parsesession import ParseSession
from PyCpp.factory import PyCppFactory, FileNameProcessor
from PyCpp.helpers import ArgParser, check_arg
from PyCpp.streams import FileStream

import sys
import glob

def main():
	argp = ArgParser(sys.argv)

	ppath, globex, output_ext = check_arg(argp)

	psess = ParseSession()
	psess.load_grammar("PyCpp/data/grammar.grm", False)

	processed_files = []
	for jfile in glob.glob(ppath + globex):

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
				print(ext, "------------------------------\n")
				print(output)

		# activate to write output to file
		for stream in active_streams:
			stream.write()

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

	# fstrm = FileStream(ppath + "/CMakeLists.txt")
	fstrm = FileStream("/CMakeLists.txt")
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

	# print(cmake.files.get_files())
	fstrm(cmake.make_header())
	fstrm(cmake.make_files())
	fstrm(cmake.make_dependencies())
	fstrm(cmake.make_builder())

	# write cmakelists file on disk
	# fstrm.write()

	# output in terminal if verbose
	if argp.get("v"):
		print(fstrm)


if __name__ == "__main__":
	main()

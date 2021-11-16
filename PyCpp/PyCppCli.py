
# import engines
from utils import PyCppEngine, CMakeGenerator, ParseSession
from utils.factory import FileNameProcessor, HelperFactory
from utils.helpers import ArgParser

# import helper functions
from utils.helpers import check_parser_input_args

# get streams
from streams import FileStream

# stb lib
import glob
import os.path
import pathlib

from typing import List

def main(argv: List[str]) -> None:
	"""
	Main function:

	1- Initializes CLI arguments parser (utils.helpers.ArgParser) and checks the arguments are valid
	while production (project path, regex glob pattern, output extensions (py,cpp,h,ctypes))

	2- Initializes ParseSession object, loads grammar and loops through glob-ed files.

	2.1.1- If a parse tree is generated (i.e. source file accepted/parsed),
	fs_fabric (~FileSystem_Fabric) creates instances of streams.FileStream and uses them to initialize
	the generators (i.e. obsersers.*) which act as observers in relation with utils.PyCppEngine

	2.1.2- PyCppEngine is initialized with the json parse tree (result of ParseSession.parse_to_json)
	and the generators. The json data are parsed into namedTuples (class, method, attribute ... at
	observers.observer) which in turns are used to stream the parsed data into the templates used in the
	generators (i.e. observers.*). The results are written to their appropriate files using the FileStream
	object.

	2.2.1- If a parse tree is not found, the error is processed and the loop continues.

	3- Once code generation is done, an CMakeLists.txt FileStream is opened and used as an observer on
	utils.CMakeGenerator object, which in turn behaves like PyCppEngine object, generates CMake
	compilation script from template, taking into account the arguments passed in CLI (i.e. project name,
	type, libs ...)

	:return: None
	"""

	argp = ArgParser(argv)

	ppath, regex_glob, output_ext = check_parser_input_args(argp)

	# arguments processing
	pname = argp.get("pname")
	ptype = argp.get("ptype")
	plibs = argp.get("plibs").split(":") if argp.get("plibs") else []
	cpp_ver = argp.get("cpp") if argp.get("cpp") in ["11", "14", "17", "20"] else "17"
	cmk_ver = argp.get("cmk") if argp.get("cmk") else "3.5"
	dbg = argp.get("dbgflg") if argp.get("dbgflg") else "-g -O0"
	rel = argp.get("relflg") if argp.get("relflg") else "-O2"

	# check for valid parameters
	assert isinstance(pname, str), \
		"Specify project name -> pname=the_name, pname value is <{}>".format(pname)

	if ptype not in ["so", "a", "x"]:
		print("Project type incorrect <{}>. Using default <so>.".format(ptype))
		ptype = "so"

	grammarpath = str(pathlib.Path(__file__).parent / "data/grammar.grm")
	psess = ParseSession()
	psess.load_grammar(grammarpath, False)

	filelist = os.path.join(ppath, "**", regex_glob)
	# filelist += os.path.join(ppath, regex_glob)
	processed_files = []
	helper_factory = HelperFactory(pname, ppath)
	for jfile in glob.glob(filelist):

		# call parselib parser
		print("parselib > processing file \"{}\"".format(jfile))
		parsed_json = psess.parse_to_json(jfile, False)
		# print(parsed_json)
		if parsed_json:
			# prepare streams and observers
			active_streams = helper_factory.file_stream_fabric(jfile, output_ext)
			observers = helper_factory.generator_fabric(jfile, output_ext, active_streams)

			# call main generator
			gen = PyCppEngine(parsed_json, observers)
			gen.drive()

			# output results
			if argp.get("v"):
				for ext, output in zip(output_ext, active_streams):
					print(ext, "------------------------------\n")
					print(output)

			# write output to file
			for stream in active_streams:
				stream.write()

			processed_files.append(jfile.replace(ppath, ""))
			os.remove(jfile)
		else:
			# track error
			if not psess:
				print("err > parse session not initialized")
			if not psess.grammar_loaded:
				print("err > grammar has not been loaded")
			print("unprocessed file is : ", psess.unprocessed_file)

	del psess

	if not processed_files:
		cmakelists_path = os.path.join(ppath, "CMakeLists.txt")
		fstrm = FileStream(cmakelists_path)
		fnproc = FileNameProcessor(processed_files, output_ext)
		cmake = CMakeGenerator(
			pname,    # project name
			ptype,    # project type
			fnproc,   # file name processor
			plibs,    # project libraries
			cmk_ver,  # CMake version
			cpp_ver,  # C++ version
			dbg,      # debug flags
			rel,      # release flags
			observers=[fstrm]
		)

		# print(cmake.files.get_files())
		cmake.drive()

		# write cmakelists file on disk
		fstrm.write()

		# output in terminal if verbose
		if argp.get("v"):
			print(fstrm)


if __name__ == "__main__":
	import sys
	main(sys.argv)


from PyCpp.parsesession import ParseSession, StringStream
from PyCpp.observers import HppGenerator, TemplGenerator, CppGenerator, GatewayGenerator, PyGwGenerator
from PyCpp import pycpp, cmakegen as cmk

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
			out.append(PyGwGenerator(stream))
		elif ext in ["impl"]:
			out.append(TemplGenerator(stream))
	return out


if __name__ == "__main__":
	psess = ParseSession()
	psess.load_grammar("PyCpp/data/grammar.grm", False)

	processed_files = []
	for jfile in glob.glob("PyCpp/data/test_srcs/*.java"):

		# call parselib parser
		print("parselib > processing file \"{}\"".format(jfile))
		parsed_json = psess.parse_to_json(jfile, False)
		# print(parsed_json)

		output_ext = ["cpp", "h", "impl", "py"]
		str_streams = ss_fabric(output_ext)
		observers = gen_fabric(output_ext, str_streams)

		# where the magic happens : json restructuring into named tuples
		gen = pycpp.PyCppEngine(parsed_json, observers)
		# call main generator
		gen.drive()

		for ext, output in zip(output_ext, str_streams):
			print(ext, "-----------------------------")
			print(output)

		processed_files.append(jfile)

	del psess

	#
	# add cmake generator call here
	#
	cmake = cmk.CMakeGenerator(processed_files)
	cmake.cleanup_ext()
	print(cmake.processed)



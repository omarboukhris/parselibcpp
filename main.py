
from PyCpp.parsesession import ParseSession
from PyCpp.observers import HppGenerator, TemplGenerator, CppGenerator
from PyCpp import pycpp

import glob

if __name__ == "__main__":
	psess = ParseSession()
	psess.load_grammar("PyCpp/data/grammar.grm", False)

	for jfile in glob.glob("PyCpp/data/test_srcs/*.java"):

		print("parselib > processing file \"{}\"".format(jfile))

		ss = psess.parse_to_json(jfile, False)
		print(ss)

		# TODO: ctypesgen, pymodgen
		# add namespace handling in grammar
		hppgen = HppGenerator(stream=print)
		cppgen = CppGenerator(stream=print)
		templ = TemplGenerator(stream=print)

		gen = pycpp.PyCppEngine(ss, observers=[hppgen, cppgen, templ])
		gen.drive()

	del psess



from PyCpp.parsesession import ParseSession
from PyCpp.observers import HppGenerator, TemplGenerator, CppGenerator
from PyCpp import pycpp

import glob

if __name__ == "__main__":
	psess = ParseSession()
	psess.load_grammar("PyCpp/data/grammar.grm", False)

	for jfile in glob.glob("PyCpp/data/test_srcs/*.java"):

		print ("parselib > processing file \"{}\"".format(jfile))

		ss = psess.parse_to_json(jfile, False)
		print(ss)

		fh = open(jfile + ".hpp", "w")
		fc = open(jfile + ".cpp", "w")
		ft = open(jfile + ".impl", "w")

		# TODO: cppgen, ctypesgen, pymodgen
		hppgen = HppGenerator(stream=fh.write)
		cppgen = CppGenerator(stream=fc.write)
		templ = TemplGenerator(stream=ft.write)

		gen = pycpp.PyCppEngine(ss, observers=[hppgen, cppgen, templ])
		gen.drive()

		fh.close()
		fc.close()
		ft.close()

	del psess


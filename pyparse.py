
from PyCpp.parsesession import ParseSession
from PyCpp.observers import HppGenerator
from PyCpp import pycpp

if __name__ == "__main__":
	psess = ParseSession()
	psess.load_grammar("PyCpp/data/grammar.grm")
	ss = psess.parse_to_json("PyCpp/data/test2.java")
	print(ss)
	hppgen = HppGenerator(stream=print)
	# TODO: cppgen, ctypesgen, pymodgen
	gen = pycpp.PyCppEngine(ss, observers=[hppgen])
	gen.drive()

	del psess


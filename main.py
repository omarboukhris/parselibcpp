
from PyCpp.parsesession import ParseSession
from PyCpp.observers import HppGenerator, TemplGenerator
from PyCpp import pycpp

if __name__ == "__main__":
	psess = ParseSession()
	psess.load_grammar("PyCpp/data/grammar.grm", False)
	ss = psess.parse_to_json("PyCpp/data/test2.java", False)
	print(ss)
	hppgen = HppGenerator(stream=print)
	templ = TemplGenerator(stream=print)
	# TODO: cppgen, ctypesgen, pymodgen
	gen = pycpp.PyCppEngine(ss, observers=[hppgen, templ])
	gen.drive()

	del psess


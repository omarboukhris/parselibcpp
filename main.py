
from PyCpp.parsesession import ParseSession, StringStream
from PyCpp.observers import HppGenerator, TemplGenerator, CppGenerator, GatewayGenerator, PyGwGenerator
from PyCpp import pycpp

import glob

if __name__ == "__main__":
	psess = ParseSession()
	psess.load_grammar("PyCpp/data/grammar.grm", False)

	for jfile in glob.glob("PyCpp/data/test_srcs/*.java"):

		# call parselib parser
		print("parselib > processing file \"{}\"".format(jfile))
		parsed_json = psess.parse_to_json(jfile, False)
		# print(parsed_json)


		# TODO: ctypesgen, pymodgen
		# string streams to hold generated code
		sshpp, sscpp, sstemp, ssgw, pygw = StringStream(), StringStream(), StringStream(), StringStream(), StringStream()
		hppgen = HppGenerator(stream=sshpp)
		cppgen = CppGenerator(stream=sscpp)
		templ = TemplGenerator(stream=sstemp)
		gwgen = GatewayGenerator(stream=ssgw)
		pygwgen = PyGwGenerator(stream=pygw)

		# where the magic happens : json restructuring into named tuples
		gen = pycpp.PyCppEngine(parsed_json, observers=[hppgen, cppgen, templ, gwgen, pygwgen])
		# call main generator
		gen.drive()

		print("Header --------------")
		print(sshpp)
		print("Source --------------")
		print(sscpp)
		print("Templt --------------")
		print(sstemp)
		print("Gatewy --------------")
		print(ssgw)
		print("Py - GW -------------")
		print(pygw)

	del psess

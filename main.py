
from PyCpp.pyparse import ParseSession

if __name__ == "__main__":
	psess = ParseSession()
	psess.load_grammar("PyCpp/data/grammar.grm")
	ss = psess.parse_to_json("PyCpp/data/test2.java")
	print(ss)

	psess.load_grammar("data/experiment/grammarvo2.grm")
	ss = psess.parse_to_json("data/experiment/grammar.source")
	print(ss)

	del psess





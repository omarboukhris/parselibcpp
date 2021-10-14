


from ctypes import cdll

import os.path

parselib = cdll.LoadLibrary("build/libparselib.so")  # change for convinience

class ParseSession:

	def __init__(self):
		self.sess = parselib.new_session()

	def load_grammar(self, filepath: str):
		if os.path.isfile(filepath) and self.sess:
			parselib.load_grammar(self.sess, filepath.encode())

	def parse_to_json(self, filepath: str):
		if os.path.isfile(filepath) and self.sess:
			parselib.store_json(self.sess, filepath.encode())

	def __del__(self):
		parselib.del_session(self.sess)
		self.sess = None


if __name__ == "__main__":
	psess = ParseSession()
	psess.load_grammar("data/grammar.grm")
	psess.parse_to_json("data/test.java")
	psess.parse_to_json("data/test2.java")

	psess.load_grammar("data/experiment/grammarvo2.grm")
#	psess.parse_to_json("data/experiment/grammar.source") # segfaults
	del psess





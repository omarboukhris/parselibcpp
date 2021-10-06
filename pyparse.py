


from ctypes import cdll

import os.path

lib = cdll.LoadLibrary("build/libparselib.so")  # change for convinience

class ParseSession:

	def __init__(self):
		self.sess = lib.new_session()

	def load_grammar(self, filepath: str):
		if os.path.isfile(filepath) and self.sess:
			lib.load_grammar(self.sess, filepath.encode())

	def parse_to_json(self, filepath: str):
		if os.path.isfile(filepath) and self.sess:
			lib.store_json(self.sess, filepath.encode())

	def __del__(self):
		lib.del_session(self.sess)
		self.sess = None


if __name__ == "__main__":
	psess = ParseSession()
	psess.load_grammar("data/grammar.grm")
	psess.parse_to_json("data/test.java")
	psess.parse_to_json("data/test2.java")
	del psess





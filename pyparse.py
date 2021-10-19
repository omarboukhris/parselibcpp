
import ctypes
import json
import os.path

parselib = ctypes.cdll.LoadLibrary("build/libparselib.so")  # change for convinience
parselib.get_json.restype = ctypes.c_char_p

class ParseSession:

	def __init__(self):
		self.sess = parselib.new_session()

	def load_grammar(self, filepath: str):
		if os.path.isfile(filepath) and self.sess:
			parselib.load_grammar(self.sess, filepath.encode())

	def parse_to_json_file(self, filepath: str):
		if os.path.isfile(filepath) and self.sess:
			parselib.store_json(self.sess, filepath.encode())

	def parse_to_json(self, filepath: str):
		if os.path.isfile(filepath) and self.sess:
			jsonstr = parselib.get_json(self.sess, filepath.encode())
			output = json.loads(jsonstr.decode())
			return output
		return None

	def __del__(self):
		parselib.del_session(self.sess)
		self.sess = None


if __name__ == "__main__":
	psess = ParseSession()
	psess.load_grammar("data/grammar.grm")
	ss = psess.parse_to_json_file("data/test.java")
#	print(ss)
	ss = psess.parse_to_json_file("data/test2.java")
#	print(ss)

#	psess.load_grammar("data/experiment/grammarvo2.grm")
#	ss = psess.parse_to_json("data/experiment/grammar.source")
#	print(ss)

	del psess





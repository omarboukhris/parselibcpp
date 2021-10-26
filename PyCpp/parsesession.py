import ctypes
import json
import os.path


class ParseSession:

	parselib = ctypes.cdll.LoadLibrary("build/libparselib.so")  # change for convinience
	parselib.get_json.restype = ctypes.c_char_p

	def __init__(self, logLevel: int = 0):
		self.sess = ParseSession.parselib.new_session(logLevel)

	def load_grammar(self, filepath: str, verbose: bool = False):
		if os.path.isfile(filepath) and self.sess:
			ParseSession.parselib.load_grammar(self.sess, filepath.encode(), verbose)

	def parse_to_json_file(self, filepath: str, verbose: bool = False):
		if os.path.isfile(filepath) and self.sess:
			ParseSession.parselib.store_json(self.sess, filepath.encode(), verbose)

	def parse_to_json(self, filepath: str, verbose: bool = False):
		if os.path.isfile(filepath) and self.sess:
			jsonstr = ParseSession.parselib.get_json(self.sess, filepath.encode(), verbose)
			output = json.loads(jsonstr.decode())
			return output
		return None

	def __del__(self):
		ParseSession.parselib.del_session(self.sess)
		self.sess = None


if __name__ == "__main__":
	psess = ParseSession()
	psess.load_grammar("PyCpp/data/grammar.grm")
	ss = psess.parse_to_json("PyCpp/data/test2.java")
	print(ss)

	psess.load_grammar("data/experiment/grammarvo2.grm")
	ss = psess.parse_to_json("data/experiment/grammar.source")
	print(ss)

	del psess

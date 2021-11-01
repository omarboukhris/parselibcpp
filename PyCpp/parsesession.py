
import ctypes
import json
import os.path


class ParseSession:
	""" ParseSession Python wrapper of C++ ParseSession
	"""

	parselib = ctypes.cdll.LoadLibrary("build/libparselib.so")
	parselib.get_json.restype = ctypes.c_char_p

	def __init__(self, log_level: int = 0):
		self.sess = ParseSession.parselib.new_session(log_level)

	def load_grammar(self, filepath: str, verbose: bool = False) -> None:
		""" Loads grammar from file

		:param filepath: str file path
		:param verbose: set to True for verbose output
		"""
		if os.path.isfile(filepath) and self.sess:
			ParseSession.parselib.load_grammar(self.sess, filepath.encode(), verbose)

	def parse_to_json_file(self, filepath: str, verbose: bool = False) -> None:
		""" Parse source file into json file. Must be called after loading grammar

		:param filepath: str file path to source code
		:param verbose: set to True for verbose
		"""
		if os.path.isfile(filepath) and self.sess:
			ParseSession.parselib.store_json(self.sess, filepath.encode(), verbose)

	def parse_to_json(self, filepath: str, verbose: bool = False):
		""" Parse source file into json data structure. Must be called after loading grammar

		:param filepath: str file path to source code
		:param verbose: set to True for verbose
		"""
		if os.path.isfile(filepath) and self.sess:
			jsonstr = ParseSession.parselib.get_json(self.sess, filepath.encode(), verbose)
			output = json.loads(jsonstr.decode())
			return output
		return None

	def __del__(self):
		""" Calls destroyer from C/ctype interface
		"""
		ParseSession.parselib.del_session(self.sess)
		self.sess = None


if __name__ == "__main__":
	psess = ParseSession()
	psess.load_grammar("data/grammar.grm")
	ss = psess.parse_to_json("data/test2.java")
	print(ss)

	del psess

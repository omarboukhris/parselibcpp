
from PyCpp.observers import HppGenerator, TemplGenerator, CppGenerator, GatewayGenerator, PyGwGenerator
from PyCpp.streams import StringStream, FileStream

class FileNameProcessor:
	"""
	Processes filenames in a list appropriately depending on
	the file to generate
	Supported extensions: cpp, h, py, ctype, impl
	"""

	def __init__(self, files: list = []):
		self.files = files
		self._cleanup_ext()

	def _cleanup_ext(self):
		out = []
		for proc in self.files:
			cln_file_name = "".join(proc.split(".")[:-1])
			out.append(cln_file_name)
		self.files = out

	def get_files(self):
		return self.files

	def make_cpp(self):
		return self._make_ext("cpp")

	def make_h(self):
		return self._make_ext("h")

	def make_impl(self):
		return self._make_ext("impl")

	def make_py(self):
		return self._make_ext("py")

	def make_gw(self):
		return self._make_prefix_ext("pyGw_", "cpp")

	def _make_ext(self, ext: str):
		return self._make_prefix_ext("", ext)

	def _make_prefix_ext(self, pref: str, ext: str):
		files = []
		if pref:
			# add prefix to the last element in path (filename)
			for f in self.files:
				fsplit = f.split("/")
				fname = pref + fsplit[-1]
				full_fname = "/".join(fsplit[:-1] + [fname])
				files.append(full_fname)
		else:
			files = self.files
		ss = "\t" + "\n\t".join(["{}.{}".format(f, ext) for f in files])
		return ss

class PyCppFactory:
	"""
	Helper factory to generate file streams, string streams and generator observers
	"""

	@staticmethod
	def single_fn_fabric(fname: str, ext: str, pref: str = ""):
		""" Single File Name Factory

		:param fname: file name
		:param ext: file extension
		:param pref: file prefix
		:return: constructed file name
		"""
		fname = "".join(fname.split(".")[:-1])
		if pref:
			# add prefix to the last element in path (filename)
			fsplit = fname.split("/")
			new_fname = pref + fsplit[-1]
			fname = "/".join(fsplit[:-1] + [new_fname])
		ss = "{}.{}".format(fname, ext)
		return ss

	@staticmethod
	def fs_fabric(fname: str, out_ext: list = []):
		""" File Stream Factory

		:param fname: file name
		:param out_ext: output extension
		:return: list of initialized file streams
		"""
		out = []
		for ext in out_ext:
			pref = ""
			if ext in ["ctype"]:
				pref = "pyGw_"
				ext = "cpp"
			if ext in ["cpp", "h", "hpp", "py", "impl", "ctype"]:
				out.append(FileStream(
					fname=PyCppFactory.single_fn_fabric(
						fname=fname,
						ext=ext,
						pref=pref
					)
				))
		return out

	@staticmethod
	def ss_fabric(out_ext: list = []):
		""" String Stream Factory

		:param out_ext: output extension
		:return: list of initialized String streams
		"""
		out = []
		for ext in out_ext:
			if ext in ["cpp", "h", "hpp", "py", "ctype", "impl"]:
				out.append(StringStream())
		return out

	@staticmethod
	def gen_fabric(out_ext: list = (), streams: list = ()):
		""" Generator Factory

		:param out_ext: output extensions
		:param streams: File or String streams to write into
		:return: list of generators to use as observers parameters in PyCppEngine
		"""
		out = []
		for ext, stream in zip(out_ext, streams):
			if ext in ["cpp"]:
				out.append(CppGenerator(stream))
			elif ext in ["h", "hpp"]:
				out.append(HppGenerator(stream))
			elif ext in ["py"]:
				out.append(PyGwGenerator(stream))
			elif ext in ["ctype"]:
				out.append(GatewayGenerator(stream))
			elif ext in ["impl"]:
				out.append(TemplGenerator(stream))
		return out


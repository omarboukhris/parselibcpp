
from .StringStream import StringStream

from typing import List

class FileStream(StringStream):
	""" File stream to write generated code into
	"""

	def __init__(self, fname: str):
		super(FileStream, self).__init__()
		self.fname = fname

	def get_file_path(self) -> List[str]:
		return self.fname.split("/")

	def write(self) -> None:
		""" Write String Stream content into a file
		:return: None
		"""
		try:
			print("pycpp info FileStream > writing {}".format(self.fname))
			with open(self.fname, "w") as fstream:
				fstream.write(self.content)
		except Exception as e:
			print("pycpp err FileStream > An unexpected exception occured : ", e)

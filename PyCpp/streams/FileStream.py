
from .StringStream import StringStream

class FileStream(StringStream):
	""" File stream to write generated code into
	"""

	def __init__(self, fname: str):
		super(FileStream, self).__init__()
		self.fname = fname

	def write(self) -> None:
		""" Write String Stream content into a file
		:return: None
		"""
		try:
			print("pycpp info FileStream > writing {}".format(self.fname))
			fstream = open(self.fname, "w")
			fstream.write(self.content)
			fstream.close()
		except Exception as e:
			print("pycpp err FileStream > An unexpected exception occured : ", e)



class CMakeGenerator:

	def __init__(self, processed: list = []):
		self.processed = processed

	def cleanup_ext(self):
		out = []
		for proc in self.processed:
			cln_file_name = "".join(proc.split(".")[:-1])
			out.append(cln_file_name)
		self.processed = out


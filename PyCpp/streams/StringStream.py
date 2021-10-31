
class StringStream:
	def __init__(self, content: str = ""):
		self.content = content

	def reset(self):
		self.content = ""

	def write(self):
		pass

	def __call__(self, t_str: str):
		self.content += t_str

	def __str__ (self):
		return self.content

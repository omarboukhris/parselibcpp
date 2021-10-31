
class StringStream:
	"""
	String Stream feeding on generated data
	Each stream passed to a generator should
	at least implement __init__ and __call__
	"""

	def __init__(self, content: str = ""):
		self.content = content

	def reset(self) -> None:
		self.content = ""

	def __call__(self, t_str: str) -> None:
		self.content += t_str

	def __str__(self) -> str:
		return self.content

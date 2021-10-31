
class StringStream:
	"""
	String Stream feeding on generated data
	Each stream passed to a generator should
	at least implement __init__, __call__, write
	and __str__
	"""

	def __init__(self, content: str = ""):
		self.content = content

	def reset(self) -> None:
		self.content = ""

	def write(self) -> None:
		pass

	def __call__(self, t_str: str) -> None:
		self.content += t_str

	def __str__(self) -> str:
		return self.content

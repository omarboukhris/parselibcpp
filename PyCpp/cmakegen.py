
from string import Template

"""
Properties:
- project name
- project type (so, a, x)
- project files
- libs to link
- min cmake version
- C++ version
- g++ flags
"""
class CMakeGenerator:

	cmk_head_templ = Template("""
cmake_minimum_required(VERSION $cmkver)
project($pname)

set(CMAKE_CXX_STANDARD $cppver)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "$dbgflags")
set(CMAKE_CXX_FLAGS_RELEASE "$relflags")
""")

	files_templ = Template("""
file(GLOB $listname
$filenames
)
""")

	def __init__(
		self,
		p_name: str,
		p_type: str,
		p_files: list = [],
		p_libs: list = [],
		cmk_version: str = "3.5",
		cpp_version: str = "17",
		dbgflg: str = "-g",
		relflg: str = "-O2"
	):
		self.name = p_name
		self.type_ = p_type
		self.files = p_files
		self.libs = p_libs

		self.cmk_ver = cmk_version
		self.cpp_ver = cpp_version

		self.dbgflg = dbgflg
		self.relflg = relflg

		self._cleanup_ext()

	def _cleanup_ext(self):
		out = []
		for proc in self.files:
			cln_file_name = "".join(proc.split(".")[:-1])
			out.append(cln_file_name)
		self.files = out

	def make_header(self):
		ss = CMakeGenerator.cmk_head_templ.substitute(
			cmkver=self.cmk_ver,
			pname=self.name,
			cppver=self.cpp_ver,
			dbgflags=self.dbgflg,
			relflags=self.relflg
		)
		return ss

	def make_files(self):
		ss = CMakeGenerator.files_templ.substitute(
			listname="SOURCE_FILES",
			filenames=self.process_file_names("cpp")
		)
		ss += CMakeGenerator.files_templ.substitute(
			listname="HEADER_FILES",
			filenames=self.process_file_names("h")
		)
		return ss

	def process_file_names(self, ext: str):
		ss = "\t" + "\n\t".join(["{}.{}".format(f, ext) for f in self.files])
		return ss

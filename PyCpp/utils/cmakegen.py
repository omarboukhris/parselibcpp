
from utils.factory import FileNameProcessor
import json
import pathlib
import os.path

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

	cmk_head_templ = Template("\n\
cmake_minimum_required(VERSION $cmkver)\n\
project($pname)\n\n\
set(CMAKE_CXX_STANDARD $cppver)\n\
set(CMAKE_CXX_STANDARD_REQUIRED ON)\n\
set(CMAKE_CXX_EXTENSIONS OFF)\n\n\
if(NOT CMAKE_BUILD_TYPE)\n\
\tset(CMAKE_BUILD_TYPE Release)\n\
endif()\n\n\
set(CMAKE_CXX_FLAGS \"-Wall -Wextra\")\n\
set(CMAKE_CXX_FLAGS_DEBUG \"$dbgflags\")\n\
set(CMAKE_CXX_FLAGS_RELEASE \"$relflags\")\n\n")

	files_templ = Template("\
file(GLOB $listname\n\
$filenames\n)\n")

	builder_templ = "\
add_library(${PROJECT_NAME} [[ptype]]\n\
\t${SOURCE_FILES} \n\
\t${HEADER_FILES}\n\
)\n\n\
target_link_libraries(\n\
\t${PROJECT_NAME}\n\
[[plibs]]\n\
)"
	x_templ = "\
add_executable(${PROJECT_NAME} main.cpp\n\
\t${SOURCE_FILES} \n\
\t${HEADER_FILES}\n\
)\n\n\
target_link_libraries(\n\
\t${PROJECT_NAME}\n\
\t[[plibs]]\n\
)"
	find_pck_templ = Template("find_package(${package_name} ${kw} ${components} ${required_state})\n")

	def __init__(
		self,
		p_name: str,
		p_type: str,
		p_files: FileNameProcessor,
		p_libs: list = [],
		cmk_version: str = "3.5",
		cpp_version: str = "17",
		dbgflg: str = "-g",
		relflg: str = "-O2"
	):
		""" Class constructor

		:param p_name: project name
		:param p_type: project type (so/a/x)
		:param p_files: project files in a FileNameProcessor object
		:param p_libs: project linked libraries
		:param cmk_version: cmake minimum version
		:param cpp_version: c++ minimum version
		:param dbgflg: Debug flags
		:param relflg: Release flags
		"""

		self.name = p_name
		self.type_ = p_type
		self.files = p_files
		self.libs = p_libs

		self.cmk_ver = cmk_version
		self.cpp_ver = cpp_version

		self.dbgflg = dbgflg
		self.relflg = relflg

	def make_header(self) -> str:
		""" Make CMakeLists.txt header

		:return: header as a string
		"""
		ss = CMakeGenerator.cmk_head_templ.substitute(
			cmkver=self.cmk_ver,
			pname=self.name,
			cppver=self.cpp_ver,
			dbgflags=self.dbgflg,
			relflags=self.relflg
		)
		return ss

	def make_files(self) -> str:
		""" Make source and header files lists

		:return: cmake code snippet as a string
		"""
		files = self.files.make_cpp() + "\n" + self.files.make_gw()
		ss = CMakeGenerator.files_templ.substitute(
			listname="SOURCE_FILES",
			filenames=files
		)
		files = self.files.make_cpp() + "\n" + self.files.make_gw()
		ss += CMakeGenerator.files_templ.substitute(
			listname="HEADER_FILES",
			filenames=files
		)
		return ss

	def make_builder(self) -> str:
		""" Make cmake building instructions

		:return: cmake building code snippet as a string
		"""
		if self.type_ in ["so", "a"]:
			ptype = ""
			if self.type_ == "so":
				ptype = "SHARED"
			elif self.type_ == "a":
				ptype = "STATIC"
			else:
				return ""
			return CMakeGenerator.builder_templ\
				.replace("[[plibs]]", self._get_libs())\
				.replace("[[ptype]]", ptype)
		elif self.type_ in ["x"]:
			return CMakeGenerator.x_templ \
				.replace("[[plibs]]", self._get_libs())

	def make_dependencies(self) -> str:
		""" :returns: str for finding libraries
		"""
		# call some kind of lib helper
		# fetches resources from rc folder or something
		# in order to fill the lib dependencies
		out = "\n"
		current_path = str(pathlib.Path(__file__).parent.parent / "data/rc_libs/")
		for lib in self.libs:
			lib_name = lib.strip()
			components = ""

			# split name and components if defined
			if lib_name.find(")") == len(lib_name) - 1:
				lib_tab = lib_name[:-1].split("(")
				lib_name = lib_tab[0]
				components = " ".join(lib_tab[1:])

			# get required state
			required = ""
			if lib_name[0] == "!":
				lib_name = lib_name[1:]
				required = "REQUIRED"

			try:
				# load parameters from json
				filepath = os.path.join(current_path, lib_name + ".json")
				fstr = open(filepath, "r")
				content = "\n".join(fstr.readlines())
				fstr.close()
				lib_params = json.loads(content)

				# check components
				if not components:
					components = lib_params["find_kw"][0]["default_components"]

				# substitute in template
				out += CMakeGenerator.find_pck_templ.substitute(
					package_name=lib_params["package_name"],
					kw=lib_params["find_kw"][0]["kw"],
					components=components,
					required_state=required
				)
				out += "\n"
			except Exception as e:
				print("(cmakegen.make_dependency) An exception occured > ", e)

		return out + "\n"

	def _get_libs(self) -> str:
		""" :returns: str for linking libraries
		"""
		out = ""
		current_path = str(pathlib.Path(__file__).parent.parent / "data/rc_libs/")
		for lib in self.libs:
			lib_name = lib.strip()
			# get name
			if lib_name.find(")") == len(lib_name)-1:
				lib_tab = lib_name[:-1].split("(")
				lib_name = lib_tab[0]

			if lib_name[0] == "!":
				lib_name = lib_name[1:]

			try:
				filepath = os.path.join(current_path, lib_name + ".json")
				fstr = open(filepath, "r")
				content = "\n".join(fstr.readlines())
				fstr.close()

				lib_params = json.loads(content)
				if "link_library" in lib_params.keys():
					out += "\t{}\n".format(lib_params["link_library"])
			except Exception as e:
				print("(cmakegen._get_libs) An exception occured > ", e)

		return out

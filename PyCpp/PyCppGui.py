
from gui.PyCppWidget import Ui_pyCppGui

from PyQt6.QtWidgets import QApplication, QWidget, QFileDialog
from PyQt6.QtCore import QCoreApplication, QMetaObject

from utils import PyCppEngine, CMakeGenerator, ParseSession
from utils.helpers import ArgParser
from utils.factory import FileNameProcessor, HelperFactory

# get streams
from streams import FileStream

import glob
import os.path
import pathlib
import distutils.dir_util as dir_util

from typing import List

class PyCppGui(QWidget, Ui_pyCppGui):
	"""
	GUI equivalent to PyCppCli
	"""

	def __init__(self, argv: List[str]):
		super(PyCppGui, self).__init__()
		self.setupUi(self)

		self.argparser = ArgParser(argv)

		self._tr = QCoreApplication.translate

		self.print_cli_help()
		self.get_project_folder()
		self.set_project_name_if_set()

		self.getfolder_pushButton.clicked.connect(self.get_project_folder)
		self.gencode_pushButton.clicked.connect(self.generate_code)

		# necessary to connect signals and slots
		QMetaObject.connectSlotsByName(self)

		self.setFixedSize(self.frameGeometry().width(), self.frameGeometry().height())

	@classmethod
	def print_cli_help(cls):
		print("CLI parameters : --ppath=project/path --pname=project_name")

	def set_project_name_if_set(self):
		pname = self.argparser.get("pname")
		if pname and type(pname) == str:
			self.projectname_lineEdit.setText(pname)
		else:
			self.projectname_lineEdit.setText("untitled_project")

	def get_project_folder(self):
		ppath = self.argparser.get("ppath")
		if ppath and type(ppath) == str:
			project_folder = ppath
		else:
			project_folder = QFileDialog.getExistingDirectory(
				self, self._tr("pyCppGui", "Open project folder"),
				"/", options=QFileDialog.Option.DontConfirmOverwrite)
		if project_folder:
			self.rootfolder_lineEdit.setText(project_folder)

	@classmethod
	def get_copy_project_folder(cls, project_path):
		if os.path.isdir(project_path):
			new_path = str(pathlib.Path(project_path)) + "_pxx"
			dir_util.copy_tree(project_path, new_path)
			project_path = new_path
		return project_path

	def generate_code(self):
		"""
		Main method:

		1- Initializes arguments from forms (e.g. project path, regex glob pattern, output extensions
		(py,cpp,h,ctypes), ...)

		2- Initializes ParseSession object, loads grammar and loops through glob-ed files.

		2.1.1- If a parse tree is generated (i.e. source file accepted/parsed),
		fs_fabric (~FileSystem_Fabric) creates instances of streams.FileStream and uses them to initialize
		the generators (i.e. obsersers.*) which act as observers in relation with utils.PyCppEngine

		2.1.2- PyCppEngine is initialized with the json parse tree (result of ParseSession.parse_to_json)
		and the generators. The json data are parsed into namedTuples (class, method, attribute ... at
		observers.observer) which in turns are used to stream the parsed data into the templates used in the
		generators (i.e. observers.*). The results are written to their appropriate files using the FileStream
		object.

		2.2.1- If a parse tree is not found, the error is processed and the loop continues.

		3- Once code generation is done, an CMakeLists.txt FileStream is opened and used as an observer on
		utils.CMakeGenerator object, which in turn behaves like PyCppEngine object, generates CMake
		compilation script from template, taking into account the arguments passed in the widgets
		(i.e. project name, type, libs ...)

		:return: None
		"""

		# get project path and regex used for globing
		ppath = self.get_copy_project_folder(self.rootfolder_lineEdit.text())
		globex = self.globex_lineEdit.text()
		print("glob regex : {}/{}".format(ppath, globex))

		# get extensions to pass to factory and project type
		# for cmake generator
		out_ext = ["cpp", "h"]
		ptype = ""
		if self.so_radioButton.isChecked():
			ptype = "so"
			if self.cgw_checkBox.isChecked():
				out_ext.append("ctype")
				if self.pygw_checkBox.isChecked():
					out_ext.append("py")
		elif self.static_radioButton.isChecked():
			ptype = "a"
		elif self.x_radioButton.isChecked():
			ptype = "x"

		# print("project type is : ", ptype)

		# cmake generator params
		pname = self.projectname_lineEdit.text()
		plibs = self.libs_lineEdit.text().split(";")
		plibs = [elmt.strip() for elmt in plibs]
		plibs = plibs if plibs != [''] else []
		cpp_ver = self.cppver_comboBox.currentText()
		cmk_ver = self.cmkver_lineEdit.text()
		rel = self.relflg_lineEdit.text()
		dbg = self.dbgflg_lineEdit.text()

		# check for valid parameters
		if type(pname) != str or len(pname) == 0:
			self.status_label.setText("Specify project name, current value is <{}>".format(pname))
			return
		if ptype not in ["so", "a", "x"]:
			self.status_label.setText("Select project type, ptype value is <{}>".format(ptype))
			return

		# init parse session with grammar
		grammarpath = str(pathlib.Path(__file__).parent / "data/grammar.grm")
		psess = ParseSession()
		psess.load_grammar(grammarpath, False)

		filelist = glob.glob(os.path.join(ppath, "**", globex))
		i, numfiles = 0, len(filelist)
		processed_files = []
		helper_factory = HelperFactory(pname, ppath)
		for jfile in filelist:

			# call parselib parser
			# print("parselib > processing file \"{}\"".format(jfile))
			self.status_label.setText("parselib > processing file \"{}\"".format(jfile))
			parsed_json = psess.parse_to_json(jfile, False)
			if parsed_json:
				# prepare streams and observers
				active_streams = helper_factory.file_stream_fabric(jfile, out_ext)
				observers = helper_factory.generator_fabric(jfile, out_ext, active_streams)

				# call main generator
				gen = PyCppEngine(parsed_json, observers)
				gen.drive()

				# activate to write output to file
				for stream in active_streams:
					stream.write()

				processed_files.append(jfile.replace(ppath, ""))
			else:
				if not psess:
					self.status_label.setText("err > parse session not initialized")
				if not psess.grammar_loaded:
					self.status_label.setText("err > grammar has not been loaded")
				self.status_label.setText("unprocessed file is : {}".format(psess.unprocessed_file))
			i += 1
			self.progressBar.setValue(int((i/numfiles)*100))

		del psess

		self.status_label.setText("pycpp > finished parsing sources, now generating CMakeLists")

		cmakelists_path = os.path.join(ppath, "CMakeLists.txt")
		fstrm = FileStream(cmakelists_path)
		# print(cmakelists_path)

		fnproc = FileNameProcessor(processed_files, out_ext)
		cmake = CMakeGenerator(
			pname,    # project name
			ptype,    # project type
			fnproc,   # file name processor
			plibs,    # project libraries
			cmk_ver,  # CMake version
			cpp_ver,  # C++ version
			dbg,      # debug flags
			rel,      # release flags
			observers=[fstrm]
		)

		cmake.drive()

		# write cmakelists file on disk
		self.status_label.setText("pycpp > finished generating CMakeLists")
		fstrm.write()
		# print(fstrm)


if __name__ == "__main__":
	import sys

	app = QApplication(sys.argv)
	ui = PyCppGui(sys.argv)
	ui.show()
	sys.exit(app.exec())

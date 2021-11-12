
from gui.PyCppWidget import Ui_pyCppGui

from PyQt6.QtWidgets import QApplication, QWidget, QFileDialog
from PyQt6.QtCore import QCoreApplication, QMetaObject

from utils.parsesession import ParseSession
from utils.factory import PyCppFactory, FileNameProcessor
from streams import FileStream

import utils.pycppeng as pycppeng
import utils.cmakegen as cmk

import glob
import os.path
import pathlib

class PyCppGui(QWidget, Ui_pyCppGui):
	"""
	GUI equivalent to PyCppCli
	"""

	def __init__(self):
		super(PyCppGui, self).__init__()
		self.setupUi(self)

		self._tr = QCoreApplication.translate

		self.get_project_folder()

		self.getfolder_pushButton.clicked.connect(self.get_project_folder)
		self.gencode_pushButton.clicked.connect(self.generate_code)

		# necessary to connect signals and slots
		QMetaObject.connectSlotsByName(self)

		self.setFixedSize(self.frameGeometry().width(), self.frameGeometry().height())

	def get_project_folder(self):
		project_folder = QFileDialog.getExistingDirectory(
			self, self._tr("pyCppGui", "Open project folder"),
			"/", options=QFileDialog.Option.DontConfirmOverwrite)
		if project_folder:
			self.rootfolder_lineEdit.setText(project_folder)

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
		ppath = self.rootfolder_lineEdit.text()
		globex = self.globex_lineEdit.text()
		print("glob regex : {}/{}".format(ppath, globex))

		# get extensions to pass to factory and project type
		# for cmake generator
		out_ext = ["cpp", "h"]
		ptype = ""
		if self.so_radioButton.isChecked():
			ptype = "so"
			if self.cgw_checkBox.isChecked():
				out_ext.append("py")
				if self.pygw_checkBox.isChecked():
					out_ext.append("ctype")
		elif self.static_radioButton.isChecked():
			ptype = "a"
		elif self.x_radioButton.isChecked():
			ptype = "x"

		print("project type is : ", ptype)

		#
		# cmake generator params
		#
		pname = self.projectname_lineEdit.text()
		plibs = self.libs_lineEdit.text().split(";")
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
		for jfile in filelist:

			# call parselib parser
			print("parselib > processing file \"{}\"".format(jfile))
			self.status_label.setText("parselib > processing file \"{}\"".format(jfile))
			parsed_json = psess.parse_to_json(jfile, False)
			if parsed_json:
				# prepare streams and observers
				active_streams = PyCppFactory.fs_fabric(jfile, out_ext)
				observers = PyCppFactory.gen_fabric(jfile, out_ext, active_streams)

				# call main generator
				gen = pycppeng.PyCppEngine(parsed_json, observers)
				gen.drive()

				# activate to write output to file
				for stream in active_streams:
					stream.write()

				processed_files.append(jfile.replace(ppath, ""))
			else:
				if not psess:
					print("err > parse session not initialized")
				if not psess.grammar_loaded:
					print("err > grammar has not been loaded")
				print("unprocessed file is : ", psess.unprocessed_file)
			i += 1
			self.progressBar.setValue(int((i/numfiles)*100))

		del psess

		self.status_label.setText("pycpp > finished parsing sources, now generating CMakeLists")

		cmakelists_path = os.path.join(ppath, "CMakeLists.txt")
		fstrm = FileStream(cmakelists_path)
		print(cmakelists_path)

		fnproc = FileNameProcessor(processed_files, out_ext)
		cmake = cmk.CMakeGenerator(
			pname,
			ptype,
			fnproc,
			plibs,
			cmk_ver,
			cpp_ver,
			dbg,
			rel,
			observers=[fstrm]
		)

		cmake.drive()

		# write cmakelists file on disk
		fstrm.write()
		print(fstrm)
		self.status_label.setText("pycpp > finished generating CMakeLists")


if __name__ == "__main__":
	import sys

	app = QApplication(sys.argv)
	ui = PyCppGui()
	ui.show()
	sys.exit(app.exec())

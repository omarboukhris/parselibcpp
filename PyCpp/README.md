
# PyCpp

This is a python project, using parselib, used to automatically generate C++ classes with python ctypes interface.

# Build instructions

for the python gateway to work, ctype needs to know where to locate the appropriate .so file.
In pyparse.py it is linked to build/parselib.so.

To build it, these are the instructions to compile it :
```
mkdir build
cd build
cmake ../..
make -j12
```

# Parselib Python interface 

The C++ parselib interface is exposed in python using ctypes.

An example of use could be 
```python
>>> from PyCpp.utils.parsesession import ParseSession

>>> psess = ParseSession()

>>> psess.load_grammar("data/test/gram.grm")
(info) now processing : data/test/gram.grm

>>> json = psess.parse_to_json("data/test/source.txt")
[===========================================================>] 100%    finished

>>> print(json)
{'S': [{'S': [{'a': ['a'], 'b': ['b']}], 'a': ['a'], 'b': ['b']}]}

>>> del psession
```

## Example

This script loops through recursively globbed files and apply TemplateGenerator on each file correctly parsed

```python
from PyCpp.parsesession import ParseSession
from PyCpp.observers import HppGenerator, TemplGenerator
from PyCpp.utils import pycppeng

import glob

if __name__ == "__main__":
	psess = ParseSession()
	psess.load_grammar("PyCpp/data/grammar.grm", False)

	for jfile in glob.glob("PyCpp/data/test_srcs/*.templ"):
		ss = psess.parse_to_json(jfile, False)

		# replace print by filestream.write to write in file
		templ = TemplGenerator(stream=print)

		gen = pycpp.PyCppEngine(ss, observers=[hppgen, templ])
		gen.drive()

	del psess
```

# PyCpp CLI

A command line interface for generating Python/C++ gateways.

Launch it with "h" or "help" options to display usage and parameters to fill:

```
$ python PyCppCli.py h

usage :
	 
		pname=projectname 
		ptype=(so|a|x) 
		glob=regex_to_glob_files
		ext=h,cpp,impl,py,ctype
		plibs="list,of,libs,sep,by,comma" 
		path="regex/to/glob"
		v h

	v is for verbose and h is for help
	If help is active, program shows this messages and exit.
	Extensions (ext) separated by <,> should not contain spaces

```
Linking external libraries is not implemented yet, placeholders in place.

pname is the generated project's name,
ptype is it's type, `so` for shared object, `a` for static library and `x` for executable,
glob represents the regex used to glob file, usually is `*.extension` like `*.cpy` or `*.java`,
ext represents the output files, `h and cpp` are for C++ source files, `impl` is a deprecated label to generate one header file with all the code, `py and ctype` are for generating respectively C and Python gateways,
path points to the folder to glob from,
v is for verbose and h is for help

# PyCpp GUI

A GUI interface has been developped using PyQt6. To run it, you need to install the dependency.

The easiest way to do it is via pip:
```
$ pip install PyQt6
```
It is highly recommended to do it in a virtual environment











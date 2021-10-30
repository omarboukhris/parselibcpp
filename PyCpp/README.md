
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

# Python interface 

The C++ parselib interface is exposed in python using ctypes.

An example of use could be 
```python
>>> from PyCpp.pyparse import ParseSession

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
from PyCpp import pycppeng

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

## Status WIP

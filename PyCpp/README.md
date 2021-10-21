
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

## Status WIP

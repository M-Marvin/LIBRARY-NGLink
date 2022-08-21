# LIBRARY-NGLink
Helper lib for using ngspice, also makes it compatible with java JNI

This is a simple library for interacting with ngspice36.
For C++/C#:
- It currently just allows to attach/detach ngspice and use its methods.
- TODO: Some functions to load and auto-reload (if they have ben edited) circuit-files.

For Java:
- Provoides JNI wrapper methods and a java native library.
- All methods are aviable in java and can be used like in C#.

Currently the library only supports Windows 64 and 32 Bit.
Linux is planed but not aviable yet.

The C++ Worspace uses VisualStudio 2019.
The Java Workspace uses EclipseIDE 2021-09.
NOTE FOR COMPILING: New compilled dlls from VisualStudio have to be copied into 
the Java workspace manualy to compile into a new .jar.

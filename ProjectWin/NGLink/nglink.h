#pragma once

/*
The main methods and constants of the lib.
Contains declarations for callbacks, constants and methods.
Does not contain any JNI related stuff, this is handled by the
machine-generated de_m_marvin_nglink_NativeNGLink.h file!
*/

// https://www.codeproject.com/articles/69965/debugging-a-jni-application-using-netbeans-and-vis

#include "ngspice.h"
#include <string>
#include <functional>

#define DLLEXPORT extern "C" __declspec(dllexport)

/* Titles from the console output sources */
#define NG_SPICE std::string("[Simmulator] ")
#define NG_CONSOLE std::string("[Console] ")
#define NG_LINK std::string("[Linker] ")

/* Callback functions from nglink */
// Called for printing log information
typedef void(_stdcall *LogFunc)(const char*);
// Called if ngspice wants to detache (MUST be executed (with detacheNGSpice) to prevent unwanted behavior)
typedef void(_stdcall*DetacheFunc)(void);
// Called to output the simmulation data, contains the actual data after the simmulation has finished
typedef void(_stdcall*ReciveVecDataFunc)(pvecvaluesall, int);
// Called before a new simmulation is started, contains information abbout the used vectors (name, size, count etc)
typedef void(_stdcall*ReciveInitDataFunc)(pvecinfoall);

/* Exported functions from nglink */
// Initialise NGLink with all callback functions
DLLEXPORT int initNGLink(LogFunc printFunc, DetacheFunc detacheFunc);
DLLEXPORT int initNGLinkFull(LogFunc printFunc, DetacheFunc detacheFunc, ReciveVecDataFunc reciveVecFunc, ReciveInitDataFunc reciveInitFunc);
// Try to load and attach the ngspice lib
DLLEXPORT int initNGSpice(const char* libName);
// Detaches the ngspice lib
DLLEXPORT int detachNGSpice();
// Executes a standard spice command
DLLEXPORT int execCommand(char* command);
// Executes a list of commands as if loaded as file
DLLEXPORT int execList(char* circListString);
// Checks if spice is running in its background thread
DLLEXPORT bool isRunning();
// Lists all aviable plots of the current spice instance
DLLEXPORT char** listPlots();
// Gets the current active plot
DLLEXPORT char* getCurrentPlot();
// Lists all aviable vectors of the given plot
DLLEXPORT char** listVecs(char* plotName);
// Get a specific vector info
DLLEXPORT pvector_info getVecInfo(char* vecName);

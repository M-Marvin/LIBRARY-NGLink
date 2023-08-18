#pragma once

/*
The main methods and constants of the lib.
Contains declarations for callbacks, constants and methods.
Does not contain any JNI related stuff, this is handled by the
machine-generated de_m_marvin_nglink_NativeNGLink.h file!
*/

#include "ngspice.h"
#include <string>
#include <functional>
#include <windows.h>

/* Titles from the console output sources */
#define NG_SPICE std::string("[Simmulator] ")
#define NG_CONSOLE std::string("[Console] ")
#define NG_LINK std::string("[Linker] ")

/* Callback functions from nglink */
// Called for printing log information
//typedef void(_stdcall *LogFunc)(const char*);
#define LogFunc std::function<void(const char*)>
// Called if ngspice wants to detache (MUST be executed (with detacheNGSpice) to prevent unwanted behavior)
//typedef void(_stdcall*DetacheFunc)(void);
#define DetachFunc std::function<void(void)>
// Called to output the simmulation data, contains the actual data after the simmulation has finished
//typedef void(_stdcall*ReciveVecDataFunc)(pvecvaluesall, int);
#define ReciveVecDataFunc std::function<void(pvecvaluesall, int)>
// Called before a new simmulation is started, contains information abbout the used vectors (name, size, count etc)
//typedef void(_stdcall*ReciveInitDataFunc)(pvecinfoall);
#define ReciveInitDataFunc std::function<void(pvecinfoall)>

class NGLink {

protected:
	HMODULE ngSpiceModule = 0;
	bool initialised = false;
	// All callback functions provided from the application
	LogFunc logPrinterCallback;
	DetachFunc detachCallback;
	ReciveVecDataFunc reciveVecData;
	ReciveInitDataFunc reciveInitData;
	// All methods exported from ngspice used by the lib
	ngSpice_Init ngSpiceInit;
	ngSpice_Command ngSpiceCommand;
	ngSpice_Circ ngSpiceCirc;
	ngSpice_running ngSpiceRunning;
	ngGet_Vec_Info ngSpiceGetVectorInfo;
	ngSpice_CurPlot ngSpiceGetCurPlot;
	ngSpice_AllPlots ngSpiceListPlots;
	ngSpice_AllVecs ngSpiceListVectors;

public:
	void logPrinter(std::string log);
	int checkNGState();

	// Initialise NGLink with all callback functions
	__declspec(dllexport) int initNGLink(LogFunc printFunc, DetachFunc detacheFunc);
	__declspec(dllexport) int initNGLinkFull(LogFunc printFunc, DetachFunc detacheFunc, ReciveVecDataFunc reciveVecFunc, ReciveInitDataFunc reciveInitFunc);
	// Checks if nglink ist inistialisted
	__declspec(dllexport) bool isInitialisated();
	// Try to load and attach the ngspice lib
	__declspec(dllexport) int initNGSpice(const char* libName);
	// Checks if ngspice is attached
	__declspec(dllexport) bool isNGSpiceAttached();
	// Detaches the ngspice lib
	__declspec(dllexport) int detachNGSpice();
	// Executes a standard spice command
	__declspec(dllexport) int execCommand(char* command);
	// Executes a list of commands as if loaded as file
	__declspec(dllexport) int loadCircuit(char* circListString);
	// Checks if spice is running in its background thread
	__declspec(dllexport) bool isRunning();
	// Lists all aviable plots of the current spice instance
	__declspec(dllexport) char** listPlots();
	// Gets the current active plot
	__declspec(dllexport) char* getCurrentPlot();
	// Lists all aviable vectors of the given plot
	__declspec(dllexport) char** listVecs(char* plotName);
	// Get a specific vector info
	__declspec(dllexport) pvector_info getVecInfo(char* vecName);

};

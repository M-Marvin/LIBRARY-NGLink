
/*
This is a library to simplify the use of ngspice.dll
This library does mainly 3 thing:
[A] - Simplify instantiating ngspice and handling of its callback's
[B] - Make it compatible with java via a JNI interface
[C] - !WORK IN PROGRESS! Make it possible to bind a single file to ngspice and auto detect changes to cause a auto-reload
*/

#include "nglink.h"
#include <iostream>
#include <functional>
#include <string>
#include <vector>

/* Helper function for handling log prints */
void NGLink::logPrinter(std::string log)
{
	if (logPrinterCallback != NULL)
	{
		logPrinterCallback(log.c_str());
	}
	else
	{
		std::cout << "[>] " << log.c_str() << std::endl;
	}
}

/* Helper function checks if nglinker and ngspice are initialized */
int NGLink::checkNGState() {
	if (!initialised)
	{
		logPrinter("nglinker is not initialised, use initNGLink first!");
		return -1;
	}

	if (ngSpiceModule != 0)
	{
		return 1;
	}
	else
	{
		logPrinter(NG_LINK + "ngspice is not attached!");
		return -2;
	}
}

/* Initialize NGLink with all callback functions */
int NGLink::initNGLink(LogFunc printFunc, DetachFunc detacheFunc)
{
	return initNGLinkFull(printFunc, detacheFunc, NULL, NULL);
}
int NGLink::initNGLinkFull(LogFunc printFunc, DetachFunc detacheFunc, ReciveVecDataFunc reciveVecFunc, ReciveInitDataFunc reciveInitFunc)
{
	detachCallback = detacheFunc;
	logPrinterCallback = printFunc;
	reciveVecData = reciveVecFunc;
	reciveInitData = reciveInitFunc;

	initialised = logPrinterCallback != NULL && detachCallback != NULL;
	if (initialised)
	{
		logPrinter(NG_LINK + "nglink initialized!");
		return 1;
	}
	else
	{
		logPrinter(NG_LINK + "failed to initialize nglink, logPrinter and detach callback's are null!");
		return 0;
	}
}


/* Checks if nglink is initialized */
bool NGLink::isInitialisated() {
	return initialised;
}

/* Try to load and attach the ngspice lib */
int NGLink::initNGSpice(const char* libName)
{
	if (!initialised)
	{
		logPrinter(NG_LINK + "nglinker is not initialized, use initNGLink first!");
		return -1;
	}

	if (ngSpiceModule == 0)
	{

		std::wstring libNameW;
		std::copy(libName, libName + strlen(libName), std::back_inserter(libNameW)); // char* wstring convert
		ngSpiceModule = LoadLibraryW(libNameW.c_str());

		if (ngSpiceModule == 0)
		{
			logPrinter(NG_LINK + "failed to load ngspice dll " + libName + "!");
			return 0;
		}
		else
		{

			ngSpiceInit = (ngSpice_Init)GetProcAddress(ngSpiceModule, "ngSpice_Init");
			ngSpiceCommand = (ngSpice_Command)GetProcAddress(ngSpiceModule, "ngSpice_Command");
			ngSpiceCirc = (ngSpice_Circ)GetProcAddress(ngSpiceModule, "ngSpice_Circ");
			ngSpiceRunning = (ngSpice_running)GetProcAddress(ngSpiceModule, "ngSpice_running");
			ngSpiceGetVectorInfo = (ngGet_Vec_Info)GetProcAddress(ngSpiceModule, "ngGet_Vec_Info");
			ngSpiceGetCurPlot = (ngSpice_CurPlot)GetProcAddress(ngSpiceModule, "ngSpice_CurPlot");
			ngSpiceListPlots = (ngSpice_AllPlots)GetProcAddress(ngSpiceModule, "ngSpice_AllPlots");
			ngSpiceListVectors = (ngSpice_AllVecs)GetProcAddress(ngSpiceModule, "ngSpice_AllVecs");

			if (ngSpiceInit == NULL) logPrinter(NG_LINK + "FAILED to load ngspice ngSpice_Init func!");
			if (ngSpiceCommand == NULL) logPrinter(NG_LINK + "FAILED to load ngspice ngSpice_Command func!");
			if (ngSpiceCirc == NULL) logPrinter(NG_LINK + "FAILED to load ngspice ngSpice_Circ func!");
			if (ngSpiceRunning == NULL) logPrinter(NG_LINK + "FAILED to load ngspice ngSpice_running func!");
			if (ngSpiceGetVectorInfo == NULL) logPrinter(NG_LINK + "FAILED to load ngspice ngGet_Vec_Info func!");
			if (ngSpiceGetCurPlot == NULL) logPrinter(NG_LINK + "FAILED to load ngspice ngSpice_CurPlot func!");
			if (ngSpiceListPlots == NULL) logPrinter(NG_LINK + "FAILED to load ngspice ngSpice_AllPlots func!");
			if (ngSpiceListVectors == NULL) logPrinter(NG_LINK + "FAILED to load ngspice ngSpice_AllVecs func!");

			if (ngSpiceInit == NULL || ngSpiceCommand == NULL || ngSpiceCirc == NULL || ngSpiceRunning == NULL || ngSpiceGetVectorInfo == NULL || ngSpiceGetCurPlot == NULL || ngSpiceListPlots == NULL || ngSpiceListVectors == NULL) {
				logPrinter(NG_LINK + "ngspice already attached!");
				return 0;
			}

			int status = ngSpiceInit(
				[](char* print, int id, void* caller) -> int {
					NGLink* nglink = (NGLink*) caller;
					nglink->logPrinter(NG_CONSOLE + std::string(print));
					return 1;
				},
				[](char* print, int id, void* caller) -> int {
					NGLink* nglink = (NGLink*) caller;
					nglink->logPrinter(NG_CONSOLE + std::string(print));
					return 1;
				},
				[](int exitStatus, NG_BOOL detachDll, NG_BOOL normalExit, int id, void* caller) -> int {
					NGLink* nglink = (NGLink*) caller;
					if (!normalExit)
					{
						nglink->logPrinter(NG_LINK + "crash of ngspice simmulator!");
					}
					else
					{
						nglink->logPrinter(NG_LINK + "ngspice wants to quit!");
					}
					if (detachDll)
					{
						nglink->logPrinter(NG_LINK + "detaching ngspice ...");
						nglink->detachNGSpice();
					} else {
						nglink->ngSpiceModule = NULL;
						nglink->ngSpiceInit = NULL;
						nglink->ngSpiceCommand = NULL;
						nglink->ngSpiceCirc = NULL;
						nglink->ngSpiceRunning = NULL;
						nglink->ngSpiceGetVectorInfo = NULL;
						nglink->ngSpiceGetCurPlot = NULL;
						nglink->ngSpiceListPlots = NULL;
						nglink->ngSpiceListVectors = NULL;
						nglink->logPrinter(NG_LINK + "ngspice detaching by itself!");
					}
					nglink->detachCallback();
					return 1;
				},
				[](pvecvaluesall data, int count, int id, void* caller) -> int {
					NGLink* nglink = (NGLink*) caller;
					if (nglink->reciveVecData != NULL) nglink->reciveVecData(data, count);
					return 1;
				},
				[](pvecinfoall data, int id, void* caller) -> int {
					NGLink* nglink = (NGLink*) caller;
					if (nglink->reciveInitData != NULL) nglink->reciveInitData(data);
					return 1;
				},
				NULL, this
			);

			if (status == 0)
			{
				logPrinter(NG_LINK + "ngspice attached!");
				return 1;
			}
			logPrinter(NG_LINK + "failed to attach ngspice!");
			return 0;
		}
	}
	else
	{
		logPrinter(NG_LINK + "ngspice is already attached!");
		return 0;
	}
}

/* Checks if ngspice is attached */
bool NGLink::isNGSpiceAttached()
{
	return ngSpiceModule != 0;
}

/* Detaches the ngspice lib */
int NGLink::detachNGSpice()
{
	if (!initialised)
	{
		logPrinter(NG_LINK + "nglinker is not initialized, use initNGLink first!");
		return -1;
	}

	if (ngSpiceModule != 0)
	{
		if (FreeLibrary(ngSpiceModule)) {
			ngSpiceModule = NULL;
			ngSpiceInit = NULL;
			ngSpiceCommand = NULL;
			ngSpiceCirc = NULL;
			ngSpiceRunning = NULL;
			ngSpiceGetVectorInfo = NULL;
			ngSpiceGetCurPlot = NULL;
			ngSpiceListPlots = NULL;
			ngSpiceListVectors = NULL;
			logPrinter(NG_LINK + "ngspice detached!");
			return 1;
		}
		logPrinter(NG_LINK + "ngspice could not be detached!");
		return 0;
	}
	else
	{
		logPrinter(NG_LINK + "ngspice is not attached!");
		return 0;
	}
}

/* Executes a standard spice command */
int NGLink::execCommand(char* command) {
	int status = checkNGState();
	if (status != 1) return status;

	return ngSpiceCommand(command) == 0 ? 1 : 0;
}

/* Executes a list of commands as if loaded as file */
int NGLink::loadCircuit(char* circListString) {
	int status = checkNGState();
	if (status != 1) return status;

	std::vector<char*> commands = {};
	char *line = strtok(circListString, "\n");
	commands.push_back(line);
	while ((line = strtok(NULL, "\n")) != NULL) {
		commands.push_back(line);
	}
	if (strcmp(commands.back(), ".end\n") != 0) {
		commands.push_back(_strdup(".end\n"));
	}
	return ngSpiceCirc(commands.data()) == 0 ? 1 : 0;
}

/* Checks if ngspice is running in its background thread */
bool NGLink::isRunning() {
	int status = checkNGState();
	if (status != 1) return 0;

	return ngSpiceRunning();
}

/* Lists all aviable plots of the current spice instance */
char** NGLink::listPlots() {
	int status = checkNGState();
	if (status != 1) return NULL;

	return ngSpiceListPlots();
}

/* Gets the current active plot */
char* NGLink::getCurrentPlot() {
	int status = checkNGState();
	if (status != 1) return NULL;

	return ngSpiceGetCurPlot();
}

/* Lists all available vectors of the given plot */
char** NGLink::listVecs(char* plotName) {
	int status = checkNGState();
	if (status != 1) return NULL;

	return ngSpiceListVectors(plotName);
}

/* Get a specific vector info */
pvector_info NGLink::getVecInfo(char* vecName) {
	int status = checkNGState();
	if (status != 1) return NULL;

	return ngSpiceGetVectorInfo(vecName);
}

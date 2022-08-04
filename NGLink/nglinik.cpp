
/*
This is a library to simplify the use of ngspice.dll
This library does mainly 3 thing:
[A] - Simplify instantiating ng spice and handling of its callbacks
[B] - Make it compatible with java via a JNI interface
[C] - !WORK IN PROGRESS! Make it possible to bind a single file to ngspice and autodetect changes to couse a auto-reload
*/

#include "pch.h"
#include "nglink.h"
#include "de_m_marvin_nglink_NativeNGLink.h"
#include "jconstants.h"
#include <iostream>
#include <functional>
#include <string>
#include <vector>

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        std::cout << "NGLink 0.1 - NGSPICE link helper libary\n\n";
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;

}

/* Static library variables */
static HMODULE ngSpiceModule;
static bool initialised = false;
// All callback functions provoided from the application
static LogFunc logPrinterCallback;
static DetacheFunc detacheCallback;
static ReciveVecDataFunc reciveVecData;
static ReciveInitDataFunc reciveInitData;
// All methods exported from ngspice used by the lib
static ngSpice_Init ngSpiceInit;
static ngSpice_Command ngSpiceCommand;
static ngSpice_Circ ngSpiceCirc;
static ngSpice_running ngSpiceRunning;
static ngGet_Vec_Info ngSpiceGetVectorInfo;
static ngSpice_CurPlot ngSpiceGetCurPlot;
static ngSpice_AllPlots ngSpiceListPlots;
static ngSpice_AllVecs ngSpiceListVectors;

/* Helper function for handeling log prints */
void logPrinter(std::string log)
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

/* Helper function checks if nglinker and ngspice are initialised */
int checkNGState() {
    if (!initialised)
    {
        logPrinter("nglinker is not initialised, use initNGLink first!");
        return -1;
    }

    if (ngSpiceModule != NULL)
    {
        return 1;
    }
    else
    {
        logPrinter(NG_LINK + "ngspice is not attached!");
        return -2;
    }
}

/* Initialise NGLink with all callback functions */
DLLEXPORT int initNGLink(LogFunc printFunc, DetacheFunc detacheFunc)
{
    return initNGLinkFull(printFunc, detacheFunc, NULL, NULL);
}
DLLEXPORT int initNGLinkFull(LogFunc printFunc, DetacheFunc detacheFunc, ReciveVecDataFunc reciveVecFunc, ReciveInitDataFunc reciveInitFunc)
{
    detacheCallback = detacheFunc;
    logPrinterCallback = printFunc;
    reciveVecData = reciveVecFunc;
    reciveInitData = reciveInitFunc;

    initialised = logPrinterCallback != NULL && detacheCallback != NULL;
    if (initialised)
    {
        logPrinter(NG_LINK + "nglink initialised!");
    }
    else
    {
        logPrinter(NG_LINK + "FAILED to initialise nglink, some importand callbacks are null!");
    }
    return initialised ? 1 : 0;
}
JNIEnv* javaEnv;
jobject javaCallback;
jmethodID javaLogFuncID;
jmethodID javaDetacheFuncID;
jmethodID javaReceiveVecFuncID;
jmethodID javaReceiveInitFuncID;
JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_initNGLink(JNIEnv* env, jobject obj, jobject icallback) {
    
    jclass callbackClass = env->FindClass("de/m_marvin/nglink/NativeNGLink$NGCallback");
    if (callbackClass == NULL) {
        logPrinter("JNI-Linking: Failed to find NGCallback class!");
        return 0;
    }
    javaEnv = env;
    javaCallback = env->NewGlobalRef(icallback);

    javaLogFuncID = env->GetMethodID(callbackClass, "log", "(Ljava/lang/String;)V");
    if (javaLogFuncID == NULL) {
        logPrinter("JNI-Linking: Failed to find log callback method!");
    }
    javaDetacheFuncID = env->GetMethodID(callbackClass, "detacheNGSpice", "()V");
    if (javaDetacheFuncID == NULL) {
        logPrinter("JNI-Linking: Failed to find detacgeNGSpice callback method!");
    }
    javaReceiveVecFuncID = env->GetMethodID(callbackClass, "reciveVecData", "(Lde/m_marvin/nglink/NativeNGLink$VectorValuesAll;I)V");
    if (javaReceiveVecFuncID == NULL) {
        logPrinter("JNI-Linking: Failed to find reciveVecData callback method!");
    }
    javaReceiveInitFuncID = env->GetMethodID(callbackClass, "reciveInitData", "(Lde/m_marvin/nglink/NativeNGLink$PlotDescription;)V");
    if (javaReceiveInitFuncID == NULL) {
        logPrinter("JNI-Linking: Failed to find reciveInitData callback method!");
    }

    if (javaLogFuncID == NULL || javaDetacheFuncID == NULL || javaReceiveVecFuncID == NULL || javaReceiveInitFuncID == NULL) {
        return 0;
    }
    
    return initNGLinkFull(
        [](const char* print) {
            javaEnv->CallVoidMethod(javaCallback, javaLogFuncID, javaEnv->NewStringUTF(print));
        },
        []() {
            javaEnv->CallVoidMethod(javaCallback, javaDetacheFuncID);
        }, 
        [](pvecvaluesall data, int vecCount) {
            jclass vectorValuesClass = javaEnv->FindClass("de/m_marvin/nglink/NativeNGLink$VectorValue");
            if (vectorValuesClass == NULL) {
                logPrinter("JNI-Linking: Failed to find VectorValue class!");
                return;
            }
            jmethodID vectorValuesConstructor = javaEnv->GetMethodID(vectorValuesClass, "<init>", "(Ljava/lang/String;DDZZ)V");
            if (vectorValuesConstructor == NULL) {
                logPrinter("JNI-Linking: Failed to find VectorValue constructor!");
                return;
            }
            jclass vectorValuesAllClass = javaEnv->FindClass("de/m_marvin/nglink/NativeNGLink$VectorValuesAll");
            if (vectorValuesAllClass == NULL) {
                logPrinter("JNI-Linking: Failed to find VectorValuesAll class!");
                return;
            }
            jmethodID vectorValuesAllConstructor = javaEnv->GetMethodID(vectorValuesAllClass, "<init>", "(II[Lde/m_marvin/nglink/NativeNGLink$VectorValue;)V");
            if (vectorValuesAllConstructor == NULL) {
                logPrinter("JNI-Linking: Failed to find VectorValuesAll constructor!");
                return;
            }

            jint vectorCount = data->veccount;
            jint vectorIndex = data->vecindex;
            
            jobjectArray vectorValuesArr = javaEnv->NewObjectArray(vecCount, vectorValuesClass, 0);
            for (int i = 0; i < vectorCount; i++) {
                pvecvalues vectorValueNative = data->vecsa[i];
                jobject vectorValue = javaEnv->NewObject(vectorValuesClass, vectorValuesConstructor,
                    javaEnv->NewStringUTF(vectorValueNative->name),
                    vectorValueNative->creal,
                    vectorValueNative->cimag,
                    vectorValueNative->is_scale,
                    vectorValueNative->is_complex
                );
                javaEnv->SetObjectArrayElement(vectorValuesArr, i, vectorValue);
            }

            jobject vectorValuesAll = javaEnv->NewObject(vectorValuesAllClass, vectorValuesAllConstructor,
                data->veccount,
                data->veccount,
                vectorValuesArr
            );

            javaEnv->CallVoidMethod(javaCallback, javaReceiveVecFuncID, vectorValuesAll, vecCount);
        },
        [](pvecinfoall data) {
            jclass plotDescClass = javaEnv->FindClass("de/m_marvin/nglink/NativeNGLink$PlotDescription");
            if (plotDescClass == NULL) {
                logPrinter("JNI-Linking: Failed to find PlotDescription class!");
                return;
            }
            jmethodID plotDescConstructor = javaEnv->GetMethodID(plotDescClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I[Lde/m_marvin/nglink/NativeNGLink$VectorDescription;)V");
            if (plotDescConstructor == NULL) {
                logPrinter("JNI-Linking: Failed to find PlotDescription constructor!");
                return;
            }
            jclass vectorDescClass = javaEnv->FindClass("de/m_marvin/nglink/NativeNGLink$VectorDescription");
            if (vectorDescClass == NULL) {
                logPrinter("JNI-Linking: Failed to find VectorDescription class!");
                return;
            }
            jmethodID vectorDescConstructor = javaEnv->GetMethodID(vectorDescClass, "<init>", "(ILjava/lang/String;Z)V");
            if (vectorDescConstructor == NULL) {
                logPrinter("JNI-Linking: Failed to find VectorDescription constructor!");
                return;
            }
            
            jstring name = javaEnv->NewStringUTF(data->name);
            jstring title = javaEnv->NewStringUTF(data->title);
            jstring date = javaEnv->NewStringUTF(data->date);
            jstring type = javaEnv->NewStringUTF(data->type);
            jint vectorCount = data->veccount;

            jobjectArray vectorDescArr = javaEnv->NewObjectArray(vectorCount, vectorDescClass, 0);
            for (int i = 0; i < vectorCount; i++) {
                pvecinfo vectorInfoNative = data->vecs[i];

                jobject vectorInfo = javaEnv->NewObject(vectorDescClass, vectorDescConstructor,
                    vectorInfoNative->number,
                    javaEnv->NewStringUTF(vectorInfoNative->vecname),
                    vectorInfoNative->is_real
                );
                javaEnv->SetObjectArrayElement(vectorDescArr, i, vectorInfo);
            }

            jobject plotDescriptor = javaEnv->NewObject(plotDescClass, plotDescConstructor,
                name,
                title,
                data,
                type,
                vectorCount,
                vectorDescArr
            );

            javaEnv->CallVoidMethod(javaCallback, javaReceiveInitFuncID, plotDescriptor);
        }
    );

}

/* Try to load and attach the ngspice lib */
DLLEXPORT int initNGSpice(const char* libName)
{
    if (!initialised)
    {
        logPrinter("nglinker is not initialised, use initNGLink first!");
        return 0;
    }

    if (ngSpiceModule == NULL)
    {
        
        std::wstring libNameW;
        std::copy(libName, libName + strlen(libName), std::back_inserter(libNameW)); // char* wstring convert
        ngSpiceModule = LoadLibraryW(libNameW.c_str());

        if (ngSpiceModule == NULL)
        {
            logPrinter(NG_LINK + "FAILED to load ngspice dll " + libName + "!");
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
                detachNGSpice();
                return 0;
            }

            logPrinter(NG_LINK + "ngspice attached!");

            int status = ngSpiceInit(
                [](char* print, int id, void* call) -> int {
                    logPrinter(NG_CONSOLE + std::string(print)); 
                    return 1; 
                },
                [](char* print, int id, void* call) -> int { 
                    logPrinter(NG_CONSOLE + std::string(print)); 
                    return 1;  
                },
                [](int exitStatus, NG_BOOL detachDll, NG_BOOL normalExit, int id, void* call) -> int {
                    if (detachDll)
                    {
                        logPrinter(NG_LINK + "ngspice wants to detache by itself!");
                    }
                    else if (!normalExit)
                    {
                        logPrinter(NG_LINK + "crash of ngspice simmulator, awaiting detaching!");
                    }
                    else
                    {
                        logPrinter(NG_LINK + "ngspice wants to quit, awaiting detaching!");
                    }
                    detacheCallback();
                    return 1; 
                },
                [](pvecvaluesall data, int count, int id, void* call) -> int {
                    if (reciveVecData != NULL) reciveVecData(data, count);
                    return 1; 
                },
                [](pvecinfoall data, int id, void* call) -> int {
                    if (reciveInitData != NULL) reciveInitData(data);
                    return 1; 
                },
                NULL, 0
            );
            
            logPrinter(NG_LINK + "DEBUG status");
            return 1;
        }
    }
    else
    {
        logPrinter(NG_LINK + "ngspice is already attached!");
        return 0;
    }
}
JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_initNGSpice(JNIEnv* env, jobject obj, jstring libName) {
    return initNGSpice(env->GetStringUTFChars(libName, 0));
}

/* Detaches the ngspice lib */
DLLEXPORT int detachNGSpice()
{
    if (!initialised)
    {
        logPrinter("nglinker is not initialised, use initNGLink first!");
        return 0;
    }

    if (ngSpiceModule != NULL)
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
        return 0;
    }
    else
    {
        logPrinter(NG_LINK + "ngspice is not attached!");
        return 0;
    }
}
JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_detachNGSpice(JNIEnv* env, jobject obj) {
    return detachNGSpice();
}

/* Executes a standard spice command */
DLLEXPORT int execCommand(char* command) {
    int status = checkNGState();
    if (status != 1) return status;

    return ngSpiceCommand(command);
}
JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_execCommand(JNIEnv* env, jobject obj, jstring command) {
    const char* commandNative = env->GetStringUTFChars(command, 0);
    return execCommand(const_cast<char*>(commandNative));
}

/* Executes a list of commands as if loaded as file */
DLLEXPORT int loadCircuit(char* circListString) {
    int status = checkNGState();
    if (status != 1) return status;
    
    std::vector<char*> commands = {};
    char *line = std::strtok(circListString, "\n");
    commands.push_back(line);
    while ((line = std::strtok(NULL, "\n")) != NULL) {
        commands.push_back(line);
    }
    return ngSpiceCirc(commands.data());
}
JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_loadCircuit(JNIEnv* env, jobject obj, jstring commandList) {
    const char* commandListNative = env->GetStringUTFChars(commandList, 0);
    return loadCircuit(const_cast<char*>(commandListNative));
}

/* Checks if ngspice is running in its background thread */
DLLEXPORT bool isRunning() {
    int status = checkNGState();
    if (status != 1) return 0;
    
    return ngSpiceRunning();
}
JNIEXPORT jboolean JNICALL Java_de_m_1marvin_nglink_NativeNGLink_isRunning(JNIEnv* env, jobject obj) {
    return isRunning();
}

/* Lists all aviable plots of the current spice instance */
DLLEXPORT char** listPlots() {
    int status = checkNGState();
    if (status != 1) return NULL;

    return ngSpiceListPlots();
}
JNIEXPORT jobjectArray JNICALL Java_de_m_1marvin_nglink_NativeNGLink_listPlots(JNIEnv* env, jobject obj) {
    return charArrPtrToStringArray(listPlots(), env);
}

/* Gets the current active plot */
DLLEXPORT char* getCurrentPlot() {
    int status = checkNGState();
    if (status != 1) return NULL;
    
    return ngSpiceGetCurPlot();
}
JNIEXPORT jstring JNICALL Java_de_m_1marvin_nglink_NativeNGLink_getCurrentPlot(JNIEnv* env, jobject obj) {
    return env->NewStringUTF(getCurrentPlot());
}

/* Lists all aviable vectors of the given plot */
DLLEXPORT char** listVecs(char* plotName) {
    int status = checkNGState();
    if (status != 1) return NULL;

    return ngSpiceListVectors(plotName);
}
JNIEXPORT jobjectArray JNICALL Java_de_m_1marvin_nglink_NativeNGLink_getVecs(JNIEnv* env, jobject obj, jstring plotName) {
    const char* plotNameNative = env->GetStringUTFChars(plotName, 0);
    char** vecNamesNative = listVecs(const_cast<char*>(plotNameNative));
    return charArrPtrToStringArray(vecNamesNative, env);
}

/* Get a specific vector info */
DLLEXPORT pvector_info getVecInfo(char* vecName) {
    int status = checkNGState();
    if (status != 1) return NULL;
    
    return ngSpiceGetVectorInfo(vecName);
}
JNIEXPORT jobject JNICALL Java_de_m_1marvin_nglink_NativeNGLink_getVec(JNIEnv* env, jobject obj, jstring vecName) {
    const char* vecNameNative = env->GetStringUTFChars(vecName, 0);
    pvector_info vectorNative = getVecInfo(const_cast<char*>(vecNameNative));

    jclass vectorInfoClass = env->FindClass("de/m_marvin/nglink/NativeNGLink$VectorInfo");
    if (vectorInfoClass == NULL) {
        logPrinter("JNI-Linking: Failed to find VectorInfo class!");
        return NULL;
    }
    jmethodID vectorInfoConstructor = env->GetMethodID(vectorInfoClass, "<init>", "(Ljava/lang/String;IS[D[Lde/m_marvin/nglink/NativeNGLink$NGComplex;I)V");
    if (vectorInfoConstructor == NULL) {
        logPrinter("JNI-Linking: Failed to find VectorInfo constructor!");
        return NULL;
    }
    jclass ngcomplexClass = env->FindClass("de/m_marvin/nglink/NativeNGLink$NGComplex");
    if (ngcomplexClass == NULL) {
        logPrinter("JNI-Linking: Failed to find NGComplex class!");
        return NULL;
    }
    jmethodID ngcomplexConstructor = env->GetMethodID(ngcomplexClass, "<init>", "(DD)V");
    if (ngcomplexConstructor == NULL) {
        logPrinter("JNI-Linking: Failed to find NGComplex constructor!");
        return NULL;
    }

    jobjectArray ngcomplexArr = NULL;
    if (vectorNative->v_compdata != NULL) {
        
        ngcomplexArr = env->NewObjectArray(vectorNative->v_length, ngcomplexClass, 0);
        for (int i = 0; i < vectorNative->v_length; i++) {
            jobject ngcomplex = env->NewObject(ngcomplexClass, ngcomplexConstructor, vectorNative->v_compdata[i].cx_real, vectorNative->v_compdata[i].cx_imag);
            env->SetObjectArrayElement(ngcomplexArr, i, ngcomplex);
        }

    }

    jdoubleArray realdataArr = env->NewDoubleArray(vectorNative->v_length);
    env->SetDoubleArrayRegion(realdataArr, 0, vectorNative->v_length, vectorNative->v_realdata);
    
    jobject vectorInfo = env->NewObject(vectorInfoClass, vectorInfoConstructor,
        env->NewStringUTF(vectorNative->v_name),
        vectorNative->v_type,
        vectorNative->v_flags,
        realdataArr,
        ngcomplexArr,
        vectorNative->v_length
    );

    return vectorInfo;
}

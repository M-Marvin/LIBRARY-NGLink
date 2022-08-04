/*
A simple helper-file for JNI
Contains some constants and helper methods.
*/

#pragma once

#include "jni.h"
#include "ngspice.h"
#define JSTRING env->FindClass("java/lang/String")

jobjectArray charArrPtrToStringArray(char** charArrPtr, JNIEnv* env);

/*
A simple helper-file for JNI
Implementation of the helper methods.
*/

#include "pch.h"
#include "jconstants.h"
#include <vector>
#include <string>
#include <iostream>

jobjectArray charArrPtrToStringArray(char** charArrPtr, JNIEnv* env) {
    std::vector<std::string> stringVec;
    while (1) {
        char* str = *(charArrPtr++);
        if (str == 0) break;
        stringVec.push_back(std::string(str));
    }
    jobjectArray stringArr = env->NewObjectArray(stringVec.size(), JSTRING, 0);
    for (size_t i = 0; i < stringVec.size(); i++) {
        jstring javaString = env->NewStringUTF(stringVec[i].c_str());
        env->SetObjectArrayElement(stringArr, i, javaString);
    }
    return stringArr;
}

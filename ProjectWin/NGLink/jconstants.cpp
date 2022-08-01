/*
A simple helper-file for JNI
Implementation of the helper methods.
*/

#include "pch.h"
#include "jconstants.h"
#include <vector>
#include <string>

jobjectArray charArrPtrToStringArray(char** charArrPtr, JNIEnv* env) {
    std::vector<std::string> stringVec(charArrPtr, charArrPtr + 4);
    jobjectArray stringArr = env->NewObjectArray(stringVec.size(), JSTRING, 0);
    for (size_t i = 0; i < stringVec.size(); i++) {
        jstring javaString = env->NewStringUTF(stringVec[i].c_str());
        env->SetObjectArrayElement(stringArr, i, javaString);
    }
    return stringArr;
}

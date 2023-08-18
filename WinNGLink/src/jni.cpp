
#include "nglink.h"
#include "de_m_marvin_nglink_NativeNGLink.h"
#include <map>

/* Helper methods to convert to JNI values */
#define JSTRING env->FindClass("java/lang/String")

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
/* End of helper methods */

static std::map<jlong, void*> jclass2cclass;

JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_initNGLink(JNIEnv* env, jobject obj, jlong classid, jobject icallback) {

	if (jclass2cclass[classid] != NULL) {
		NGLink* nglink = (NGLink*) jclass2cclass[classid];
		nglink->logPrinter(NG_LINK + "for this class an native instance is already created!");
		return 0;
	}

	NGLink* nglink = new NGLink();
	jclass2cclass[classid] = nglink;

    jclass callbackClass = env->FindClass("de/m_marvin/nglink/NativeNGLink$INGCallback");
    if (callbackClass == NULL) {
        nglink->logPrinter(NG_LINK + "Failed to find NGCallback class!");
        return -3;
    }
    JNIEnv* javaEnv = env;
    jobject javaCallback = env->NewGlobalRef(icallback);

    jmethodID javaLogFuncID = env->GetMethodID(callbackClass, "log", "(Ljava/lang/String;)V");
    if (javaLogFuncID == NULL) {
    	nglink->logPrinter(NG_LINK + "Failed to find log callback method!");
    }
    jmethodID javaDetacheFuncID = env->GetMethodID(callbackClass, "detacheNGSpice", "()V");
    if (javaDetacheFuncID == NULL) {
    	nglink->logPrinter(NG_LINK + "Failed to find detacgeNGSpice callback method!");
    }
    jmethodID javaReceiveVecFuncID = env->GetMethodID(callbackClass, "reciveVecData", "(Lde/m_marvin/nglink/NativeNGLink$VectorValuesAll;I)V");
    if (javaReceiveVecFuncID == NULL) {
    	nglink->logPrinter(NG_LINK + "Failed to find reciveVecData callback method!");
    }
    jmethodID javaReceiveInitFuncID = env->GetMethodID(callbackClass, "reciveInitData", "(Lde/m_marvin/nglink/NativeNGLink$PlotDescription;)V");
    if (javaReceiveInitFuncID == NULL) {
    	nglink->logPrinter(NG_LINK + "Failed to find reciveInitData callback method!");
    }

    if (javaLogFuncID == NULL || javaDetacheFuncID == NULL || javaReceiveVecFuncID == NULL || javaReceiveInitFuncID == NULL) {
        return -3;
    }

    return nglink->initNGLinkFull(
        [javaEnv, javaCallback, javaLogFuncID](const char* print) {
            javaEnv->CallVoidMethod(javaCallback, javaLogFuncID, javaEnv->NewStringUTF(print));
        },
        [javaEnv, javaCallback, javaDetacheFuncID]() {
            javaEnv->CallVoidMethod(javaCallback, javaDetacheFuncID);
        },
        [nglink, javaEnv, javaCallback, javaReceiveVecFuncID](pvecvaluesall data, int vecCount) {
            jclass vectorValuesClass = javaEnv->FindClass("de/m_marvin/nglink/NativeNGLink$VectorValue");
            if (vectorValuesClass == NULL) {
            	nglink->logPrinter(NG_LINK + "Failed to find VectorValue class!");
                return;
            }
            jmethodID vectorValuesConstructor = javaEnv->GetMethodID(vectorValuesClass, "<init>", "(Ljava/lang/String;DDZZ)V");
            if (vectorValuesConstructor == NULL) {
            	nglink->logPrinter(NG_LINK + "Failed to find VectorValue constructor!");
                return;
            }
            jclass vectorValuesAllClass = javaEnv->FindClass("de/m_marvin/nglink/NativeNGLink$VectorValuesAll");
            if (vectorValuesAllClass == NULL) {
            	nglink->logPrinter(NG_LINK + "Failed to find VectorValuesAll class!");
                return;
            }
            jmethodID vectorValuesAllConstructor = javaEnv->GetMethodID(vectorValuesAllClass, "<init>", "(II[Lde/m_marvin/nglink/NativeNGLink$VectorValue;)V");
            if (vectorValuesAllConstructor == NULL) {
            	nglink->logPrinter(NG_LINK + "Failed to find VectorValuesAll constructor!");
                return;
            }

            jint vectorCount = data->veccount;
            //jint vectorIndex = data->vecindex;

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
        [nglink, javaEnv, javaCallback, javaReceiveInitFuncID](pvecinfoall data) {
            jclass plotDescClass = javaEnv->FindClass("de/m_marvin/nglink/NativeNGLink$PlotDescription");
            if (plotDescClass == NULL) {
            	nglink->logPrinter(NG_LINK + "Failed to find PlotDescription class!");
                return;
            }
            jmethodID plotDescConstructor = javaEnv->GetMethodID(plotDescClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I[Lde/m_marvin/nglink/NativeNGLink$VectorDescription;)V");
            if (plotDescConstructor == NULL) {
            	nglink->logPrinter(NG_LINK + "Failed to find PlotDescription constructor!");
                return;
            }
            jclass vectorDescClass = javaEnv->FindClass("de/m_marvin/nglink/NativeNGLink$VectorDescription");
            if (vectorDescClass == NULL) {
            	nglink->logPrinter(NG_LINK + "Failed to find VectorDescription class!");
                return;
            }
            jmethodID vectorDescConstructor = javaEnv->GetMethodID(vectorDescClass, "<init>", "(ILjava/lang/String;Z)V");
            if (vectorDescConstructor == NULL) {
            	nglink->logPrinter(NG_LINK + "Failed to find VectorDescription constructor!");
                return;
            }

            jstring name = javaEnv->NewStringUTF(data->name);
            jstring title = javaEnv->NewStringUTF(data->title);
            //jstring date = javaEnv->NewStringUTF(data->date);
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

JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_detachNGLink(JNIEnv* env, jobject obj, jlong classid) {
	NGLink* nglink = (NGLink*) jclass2cclass[classid];
	if (nglink == NULL) return -3;
	if (nglink->isNGSpiceAttached()) {
		nglink->detachNGSpice();
	}
	nglink->logPrinter(NG_LINK + "detaching nglink!");
	delete nglink;
	jclass2cclass[classid] = NULL;
	return 1;
}

JNIEXPORT jboolean JNICALL Java_de_m_1marvin_nglink_NativeNGLink_isInitialized(JNIEnv* env, jobject obj, jlong classid) {
	NGLink* nglink = (NGLink*) jclass2cclass[classid];
    if (nglink == NULL) return false;
    return nglink->isInitialisated() == 1;
}

JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_initNGSpice(JNIEnv* env, jobject obj, jlong classid, jstring libName) {
	NGLink* nglink = (NGLink*) jclass2cclass[classid];
    if (nglink == NULL) return -3;
	if (libName == NULL) return 0;
    return nglink->initNGSpice(env->GetStringUTFChars(libName, 0));
}

JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_detachNGSpice(JNIEnv* env, jobject obj, jlong classid) {
	NGLink* nglink = (NGLink*) jclass2cclass[classid];
    if (nglink == NULL) return -3;
    return nglink->detachNGSpice();
}

jboolean Java_de_m_1marvin_nglink_NativeNGLink_isNGSpiceAttached(JNIEnv* env, jobject obj, jlong classid) {
	NGLink* nglink = (NGLink*) jclass2cclass[classid];
	if (nglink == NULL) return false;
	return nglink->isNGSpiceAttached();
}

JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_execCommand(JNIEnv* env, jobject obj, jlong classid, jstring command) {
	NGLink* nglink = (NGLink*) jclass2cclass[classid];
    if (nglink == NULL) return -3;
    const char* commandNative = env->GetStringUTFChars(command, 0);
    return nglink->execCommand(const_cast<char*>(commandNative));
}

JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_loadCircuit(JNIEnv* env, jobject obj, jlong classid, jstring commandList) {
	NGLink* nglink = (NGLink*) jclass2cclass[classid];
    if (nglink == NULL) return -3;
    if (commandList == NULL) return 0;
    const char* commandListNative = env->GetStringUTFChars(commandList, 0);
    return nglink->loadCircuit(const_cast<char*>(commandListNative));
}

JNIEXPORT jboolean JNICALL Java_de_m_1marvin_nglink_NativeNGLink_isRunning(JNIEnv* env, jobject obj, jlong classid) {
	NGLink* nglink = (NGLink*) jclass2cclass[classid];
    if (nglink == NULL) return false;
    return nglink->isRunning();
}

JNIEXPORT jobjectArray JNICALL Java_de_m_1marvin_nglink_NativeNGLink_listPlots(JNIEnv* env, jobject obj, jlong classid) {
	NGLink* nglink = (NGLink*) jclass2cclass[classid];
    if (nglink == NULL) return NULL;
	char** plots = nglink->listPlots();
    return plots == NULL ? NULL : charArrPtrToStringArray(plots, env);
}

JNIEXPORT jstring JNICALL Java_de_m_1marvin_nglink_NativeNGLink_getCurrentPlot(JNIEnv* env, jobject obj, jlong classid) {
	NGLink* nglink = (NGLink*) jclass2cclass[classid];
    if (nglink == NULL) return NULL;
	char* plot = nglink->getCurrentPlot();
    return plot == NULL ? NULL : env->NewStringUTF(plot);
}

JNIEXPORT jobjectArray JNICALL Java_de_m_1marvin_nglink_NativeNGLink_getVecs(JNIEnv* env, jobject obj, jlong classid, jstring plotName) {
	NGLink* nglink = (NGLink*) jclass2cclass[classid];
    if (nglink == NULL) return NULL;
    const char* plotNameNative = env->GetStringUTFChars(plotName, 0);
    char** vecNamesNative = nglink->listVecs(const_cast<char*>(plotNameNative));
    return vecNamesNative == NULL ? NULL : charArrPtrToStringArray(vecNamesNative, env);
}

JNIEXPORT jobject JNICALL Java_de_m_1marvin_nglink_NativeNGLink_getVec(JNIEnv* env, jobject obj, jlong classid, jstring vecName) {
	NGLink* nglink = (NGLink*) jclass2cclass[classid];
    if (nglink == NULL) return NULL;

    const char* vecNameNative = env->GetStringUTFChars(vecName, 0);
    pvector_info vectorNative = nglink->getVecInfo(const_cast<char*>(vecNameNative));

    jclass vectorInfoClass = env->FindClass("de/m_marvin/nglink/NativeNGLink$VectorInfo");
    if (vectorInfoClass == NULL) {
    	nglink->logPrinter(NG_LINK + "Failed to find VectorInfo class!");
        return NULL;
    }
    jmethodID vectorInfoConstructor = env->GetMethodID(vectorInfoClass, "<init>", "(Ljava/lang/String;IS[D[Lde/m_marvin/nglink/NativeNGLink$NGComplex;I)V");
    if (vectorInfoConstructor == NULL) {
    	nglink->logPrinter(NG_LINK + "Failed to find VectorInfo constructor!");
        return NULL;
    }
    jclass ngcomplexClass = env->FindClass("de/m_marvin/nglink/NativeNGLink$NGComplex");
    if (ngcomplexClass == NULL) {
    	nglink->logPrinter(NG_LINK + "Failed to find NGComplex class!");
        return NULL;
    }
    jmethodID ngcomplexConstructor = env->GetMethodID(ngcomplexClass, "<init>", "(DD)V");
    if (ngcomplexConstructor == NULL) {
    	nglink->logPrinter(NG_LINK + "Failed to find NGComplex constructor!");
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

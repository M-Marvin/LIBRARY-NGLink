/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class de_m_marvin_nglink_NativeNGLink */

#ifndef _Included_de_m_marvin_nglink_NativeNGLink
#define _Included_de_m_marvin_nglink_NativeNGLink
#ifdef __cplusplus
extern "C" {
#endif
	/*
	 * Class:     de_m_marvin_nglink_NativeNGLink
	 * Method:    initNGLink
	 * Signature: (Lde/m_marvin/nglink/NativeNGLink/NGCallback;)I
	 */
	JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_initNGLink
	(JNIEnv*, jobject, jobject);

	/*
	 * Class:     de_m_marvin_nglink_NativeNGLink
	 * Method:    initNGSPice
	 * Signature: (Ljava/lang/String;)I
	 */
	JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_initNGSPice
	(JNIEnv*, jobject, jstring);

	/*
	 * Class:     de_m_marvin_nglink_NativeNGLink
	 * Method:    detachNGSpice
	 * Signature: ()I
	 */
	JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_detachNGSpice
	(JNIEnv*, jobject);

	/*
	 * Class:     de_m_marvin_nglink_NativeNGLink
	 * Method:    execCommand
	 * Signature: (Ljava/lang/String;)I
	 */
	JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_execCommand
	(JNIEnv*, jobject, jstring);

	/*
	 * Class:     de_m_marvin_nglink_NativeNGLink
	 * Method:    execList
	 * Signature: (Ljava/lang/String;)I
	 */
	JNIEXPORT jint JNICALL Java_de_m_1marvin_nglink_NativeNGLink_execList
	(JNIEnv*, jobject, jstring);

	/*
	 * Class:     de_m_marvin_nglink_NativeNGLink
	 * Method:    isRunning
	 * Signature: ()Z
	 */
	JNIEXPORT jboolean JNICALL Java_de_m_1marvin_nglink_NativeNGLink_isRunning
	(JNIEnv*, jobject);

	/*
	 * Class:     de_m_marvin_nglink_NativeNGLink
	 * Method:    listPlots
	 * Signature: ()[Ljava/lang/String;
	 */
	JNIEXPORT jobjectArray JNICALL Java_de_m_1marvin_nglink_NativeNGLink_listPlots
	(JNIEnv*, jobject);

	/*
	 * Class:     de_m_marvin_nglink_NativeNGLink
	 * Method:    getCurrentPlot
	 * Signature: ()Ljava/lang/String;
	 */
	JNIEXPORT jstring JNICALL Java_de_m_1marvin_nglink_NativeNGLink_getCurrentPlot
	(JNIEnv*, jobject);

	/*
	 * Class:     de_m_marvin_nglink_NativeNGLink
	 * Method:    getVecs
	 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
	 */
	JNIEXPORT jobjectArray JNICALL Java_de_m_1marvin_nglink_NativeNGLink_getVecs
	(JNIEnv*, jobject, jstring);

	/*
	 * Class:     de_m_marvin_nglink_NativeNGLink
	 * Method:    getVec
	 * Signature: (Ljava/lang/String;)Lde/m_marvin/nglink/NativeNGLink/VectorInfo;
	 */
	JNIEXPORT jobject JNICALL Java_de_m_1marvin_nglink_NativeNGLink_getVec
	(JNIEnv*, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif

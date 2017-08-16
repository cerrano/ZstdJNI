
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "libJNIZstdWin.h"
#include "jni.h"
#include "../zstd/zstd.h"

#define MaxBuffLen				32768
//
char* g_bufCompress = NULL;
int g_bufCompressLen = 0;
char* g_bufDecompress = NULL;
int g_bufDecompressLen = 0;

extern "C"
JNIEXPORT void JNICALL
Java_com_mitake_util_ZstdWin_print(JNIEnv *env, jobject thiz)
{
	printf("Zstd Hello world\n");
	return;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_mitake_util_ZstdWin_init(JNIEnv *env, jobject thiz)
{
	if (g_bufCompress)
	{
		free(g_bufCompress);
		g_bufCompress = NULL;
	}
	g_bufCompressLen = 0;
	g_bufCompress = (char*)malloc(MaxBuffLen);
	if (!g_bufCompress)
		return false;
	g_bufCompressLen = MaxBuffLen;

	if (g_bufDecompress)
	{
		free(g_bufDecompress);
		g_bufDecompress = NULL;
	}
	g_bufDecompressLen = 0;
	g_bufDecompress = (char*)malloc(MaxBuffLen);
	if (!g_bufDecompress)
	{
		free(g_bufCompress);
		g_bufCompress = NULL;
		return false;
	}
	g_bufDecompressLen = MaxBuffLen;

	return true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_mitake_util_ZstdWin_fini(JNIEnv *env, jobject thiz)
{
	if (g_bufCompress)
	{
		free(g_bufCompress);
		g_bufCompress = NULL;
	}
	g_bufCompressLen = 0;

	if (g_bufDecompress)
	{
		free(g_bufDecompress);
		g_bufDecompress = NULL;
	}
	g_bufDecompressLen = 0;

	return;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_mitake_util_ZstdWin_compress(JNIEnv *env, jobject thiz, jbyteArray ary)
{
	if (!ary)
		return NULL;

	jsize len = env->GetArrayLength(ary);
	jbyte* jb = env->GetByteArrayElements(ary, 0);
	unsigned char* arrayBody = (unsigned char*)jb;
	if (!arrayBody)
		return NULL;

	size_t const cBuffSize = ZSTD_compressBound(len);
	if (g_bufCompressLen < cBuffSize)
	{
		free(g_bufCompress);

		g_bufCompress = (char*)malloc(cBuffSize);

		g_bufCompressLen = cBuffSize;
	}
	if (!g_bufCompress)
	{
		env->ReleaseByteArrayElements(ary, jb, 0);
		g_bufCompressLen = 0;
		return NULL;
	}

	size_t const cSize = ZSTD_compress(g_bufCompress, cBuffSize, arrayBody, len, 1);
	env->ReleaseByteArrayElements(ary, jb, 0);
	if (ZSTD_isError(cSize) == 0)
	{
		jbyteArray bArray = env->NewByteArray(cSize);
		env->SetByteArrayRegion(bArray, 0, cSize, (jbyte*)g_bufCompress);

		return (jbyteArray)bArray;
	}

	return NULL;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_mitake_util_ZstdWin_decompress(JNIEnv *env, jobject thiz, jbyteArray ary)
{
	if (!ary)
		return NULL;

	jsize len = env->GetArrayLength(ary);
	jbyte* jb = env->GetByteArrayElements(ary, 0);
	unsigned char* arrayBody = (unsigned char*)jb;
	if (!arrayBody)
		return NULL;

	unsigned long long const rSize = ZSTD_getDecompressedSize(arrayBody, len);
	if (rSize > 0)
	{
		if (g_bufDecompressLen < rSize)
		{
			free(g_bufDecompress);

			g_bufDecompress = (char*)malloc(rSize);

			g_bufDecompressLen = rSize;
		}
		if (!g_bufDecompress)
		{
			env->ReleaseByteArrayElements(ary, jb, 0);
			g_bufDecompressLen = 0;
			return NULL;
		}

		size_t const dSize = ZSTD_decompress(g_bufDecompress, rSize, arrayBody, len);
		env->ReleaseByteArrayElements(ary, jb, 0);
		if (dSize == rSize)
		{
			jbyteArray bArray = env->NewByteArray(rSize);
			env->SetByteArrayRegion(bArray, 0, rSize, (jbyte*)g_bufDecompress);

			return (jbyteArray)bArray;
		}
	}
	else
		env->ReleaseByteArrayElements(ary, jb, 0);
	return NULL;
}

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_mitake_util_ZstdWin_Native(JNIEnv* env, jobject thiz)
{
	// Enter code here
}

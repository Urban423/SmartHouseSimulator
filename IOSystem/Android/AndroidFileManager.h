#pragma once
#include "../IOstructures.h"
#include <jni.h>

class FileManager
{
public:
	FileManager();
	
	static void init(JNIEnv* env, jobject* java_obj);
	static CFile readFile(const char* fileName);
	static void print(const char* text);
public:
	static FileManager* androidFileManager;

	JNIEnv* env;
	jobject* java_obj;
	jclass cls;
	jmethodID printTextID;
	jmethodID readFileID;
};
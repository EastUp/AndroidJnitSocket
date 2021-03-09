//
// Created by Administrator on 2017/3/3 0003.
//

#ifndef ANDROIDJNITSOCKET_STRINGTOOS_H
#define ANDROIDJNITSOCKET_STRINGTOOS_H


#include <jni.h>

class StringTools {
public:
    static char* jstringTostring(JNIEnv* env, jstring jstr);
};


#endif //ANDROIDJNITSOCKET_STRINGTOOS_H

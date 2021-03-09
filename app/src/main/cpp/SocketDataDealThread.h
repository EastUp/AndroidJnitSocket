//
// Created by Administrator on 2017/3/3 0003.
//

#ifndef ANDROIDJNITSOCKET_SOCKETDATADEALTHREAD_H
#define ANDROIDJNITSOCKET_SOCKETDATADEALTHREAD_H


#include <pthread.h>

class SocketDataDealThread {
public:
    static void * clientThread(void *args);
    static pthread_cond_t cond;
    static JavaVM*  javavm;
    static pthread_mutex_t mutex;
    static bool isShoudExit;
    static JNIEnv* env;
    static jobject obj;

    SocketDataDealThread(int fd,jobject obj);
    ~SocketDataDealThread();
    void sendData(char buff[],int length);
    void setIsShoudExit(bool isShoud);
    pthread_t getSocketThreadId();
    void wakeUpThread();

private:
    pthread_t threadId;
    static int socketFd;
    static char * getBuffer;
    char * sendBuffer;
    int sendLength=0;
};


#endif //ANDROIDJNITSOCKET_SOCKETDATADEALTHREAD_H

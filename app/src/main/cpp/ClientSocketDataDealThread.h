//
// Created by Administrator on 2017/3/3 0003.
//

#ifndef ANDROIDJNITSOCKET_CLIENTSOCKETDATADEALTHREAD_H
#define ANDROIDJNITSOCKET_CLIENTSOCKETDATADEALTHREAD_H


#include <pthread.h>

typedef struct _SP_SendToClintStream_INFO
{
    char pData_0[3000000];
    char pData_1[750000];
    char pData_2[750000];
    int _Width;
    int _Height;
    int Linesize_0;
    int Linesize_1;
    int Linesize_2;
} SP_SendToClintStream_INFO, *PSP_SendToClintStream_INFO;

class ClientSocketDataDealThread {
public:
    static void * clientThread(void *args);
    static pthread_cond_t cond;
    static JavaVM*  javavm;
    static pthread_mutex_t mutex;
    static bool isShoudExit;
    static JNIEnv* env;
    static jobject obj;

    ClientSocketDataDealThread(int fd,jobject obj);
    ~ClientSocketDataDealThread();
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


#endif //ANDROIDJNITSOCKET_CLIENTSOCKETDATADEALTHREAD_H

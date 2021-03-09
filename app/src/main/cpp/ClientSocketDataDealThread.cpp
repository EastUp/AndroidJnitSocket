//
// Created by Administrator on 2017/3/3 0003.
//
#include <sys/socket.h>
#include <jni.h>
#include <malloc.h>
#include <memory.h>
#include "my_log.h"
#include "ClientSocketDataDealThread.h"

pthread_cond_t ClientSocketDataDealThread::cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t ClientSocketDataDealThread::mutex = PTHREAD_MUTEX_INITIALIZER;
int ClientSocketDataDealThread::socketFd = 0;
char *ClientSocketDataDealThread::getBuffer = NULL;
bool ClientSocketDataDealThread::isShoudExit = false;
JNIEnv *ClientSocketDataDealThread::env = NULL;
jobject ClientSocketDataDealThread::obj = NULL;
JavaVM *ClientSocketDataDealThread::javavm = NULL;

void *ClientSocketDataDealThread::clientThread(void *args) {
    javavm->AttachCurrentThread(&ClientSocketDataDealThread::env, NULL);

    LOGI("%s:ClientSocketDataDealThread is running", TAG);
    while (!ClientSocketDataDealThread::isShoudExit) {
//        pthread_mutex_lock(&ClientSocketDataDealThread::mutex);
//        pthread_cond_wait(&ClientSocketDataDealThread::cond,&ClientSocketDataDealThread::mutex);
//        pthread_mutex_unlock(&ClientSocketDataDealThread::mutex);
        LOGI("%s:clientThread wake ", TAG);

        _SP_SendToClintStream_INFO *myNode = (_SP_SendToClintStream_INFO *) malloc(
                sizeof(_SP_SendToClintStream_INFO));

        int needRecv = sizeof(_SP_SendToClintStream_INFO);
        char *buffer = (char *) malloc(needRecv);
        int pos = 0;
        int len;

        while (pos < needRecv) {
            LOGI("%s:needRecv=%d", TAG,needRecv);
            len = recv(socketFd, buffer + pos,needRecv, 0);
            LOGI("%s:len = %d", TAG, len);
            if (len < 0) {
                LOGI("%s:Server Recieve Data Failed!", TAG);
                printf("Server Recieve Data Failed!\n");
//                break;
            }
            pos += len;
        }
//        close(new_server_socket);
        memcpy(myNode, buffer, needRecv);
        LOGI("%s:recv over Width=%d Height=%d\n",TAG, myNode->_Width, myNode->_Height);
//        free(buffer);
//        free(myNode);

//        int len = recv(socketFd,getBuffer, sizeof(getBuffer),0);
        LOGI("%s:get data %s,len = %d", TAG, buffer, len);
        if (ClientSocketDataDealThread::env != NULL && ClientSocketDataDealThread::obj) {
            jclass cls = ClientSocketDataDealThread::env->GetObjectClass(
                    ClientSocketDataDealThread::obj);
            //           if(cls == NULL){
            //               LOGI("%s:find class error",TAG);
            //               pthread_exit(NULL);
            //           }
            jmethodID mid = ClientSocketDataDealThread::env->GetMethodID(cls, "setRecevieData",
                                                                         "([B)V");
            ClientSocketDataDealThread::env->DeleteLocalRef(cls);
            if (mid == NULL) {
                LOGI("%s:find method1 error", TAG);
                pthread_exit(NULL);
            }
            jbyteArray jarray = ClientSocketDataDealThread::env->NewByteArray(len);
            ClientSocketDataDealThread::env->SetByteArrayRegion(jarray, 0, len, (jbyte *) buffer);
            ClientSocketDataDealThread::env->CallVoidMethod(obj, mid, jarray);
            ClientSocketDataDealThread::env->DeleteLocalRef(jarray);
        }
    }
    LOGI("%s:ClientSocketDataDealThread exit", TAG);
    return NULL;
}

ClientSocketDataDealThread::ClientSocketDataDealThread(int fd, jobject obj1) :
        threadId(0), sendLength(0) {
    pthread_mutex_init(&mutex, 0);
    pthread_cond_init(&cond, 0);
    socketFd = fd;
    ClientSocketDataDealThread::obj = obj1;
    getBuffer = new char[100];
    sendBuffer = new char[100];
    if (pthread_create(&threadId, NULL, ClientSocketDataDealThread::clientThread, NULL) != 0) {
        LOGI("%s:pthread_create error", TAG);
    }
    LOGI("%s:mSTh->getSocketThreadId():%lu", TAG, (long) threadId);
}

ClientSocketDataDealThread::~ClientSocketDataDealThread() {
    delete getBuffer;
    delete sendBuffer;
}

void ClientSocketDataDealThread::sendData(char *buff, int length) {
    LOGI("%s:send data %s,len = %d", TAG, buff, length);
    int len = send(socketFd, buff, length, 0);
    if (len < 0) {
        LOGI("%s:send data error,len = %d", TAG, len);
    }
    wakeUpThread();
}

pthread_t ClientSocketDataDealThread::getSocketThreadId() {
    return threadId;
}

void ClientSocketDataDealThread::wakeUpThread() {
    pthread_mutex_lock(&ClientSocketDataDealThread::mutex);
    // 设置条件为真
    pthread_cond_signal(&ClientSocketDataDealThread::cond);
    pthread_mutex_unlock(&ClientSocketDataDealThread::mutex);
}


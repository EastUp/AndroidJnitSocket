//
// Created by Administrator on 2017/3/3 0003.
//
#include <sys/socket.h>
#include <jni.h>
#include "my_log.h"
#include "SocketDataDealThread.h"
pthread_cond_t SocketDataDealThread::cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t SocketDataDealThread::mutex = PTHREAD_MUTEX_INITIALIZER;
int SocketDataDealThread::socketFd = 0;
char * SocketDataDealThread::getBuffer = NULL;
bool SocketDataDealThread::isShoudExit = false;
JNIEnv* SocketDataDealThread::env=NULL;
jobject SocketDataDealThread::obj=NULL;
JavaVM*  SocketDataDealThread::javavm = NULL;

void *SocketDataDealThread::clientThread(void *args) {
    javavm->AttachCurrentThread(&SocketDataDealThread::env,NULL);

    LOGI("%s:SocketDataDealThread is running",TAG);
    while (!SocketDataDealThread::isShoudExit){
        pthread_mutex_lock(&SocketDataDealThread::mutex);
        pthread_cond_wait(&SocketDataDealThread::cond,&SocketDataDealThread::mutex);
        pthread_mutex_unlock(&SocketDataDealThread::mutex);
        LOGI("%s:clientThread wake ",TAG);
        int len = recv(socketFd,getBuffer, sizeof(getBuffer),0);
        LOGI("%s:get data %s,len = %d",TAG,getBuffer,len);
       if(SocketDataDealThread::env!=NULL && SocketDataDealThread::obj){
        jclass cls = SocketDataDealThread::env->GetObjectClass(SocketDataDealThread::obj);
        //           if(cls == NULL){
        //               LOGI("%s:find class error",TAG);
        //               pthread_exit(NULL);
        //           }
        jmethodID mid = SocketDataDealThread::env->GetMethodID(cls,"setRecevieData","([B)V");
        SocketDataDealThread::env->DeleteLocalRef(cls);
        if(mid==NULL){
            LOGI("%s:find method1 error",TAG);
            pthread_exit(NULL);
        }
        jbyteArray jarray = SocketDataDealThread::env->NewByteArray(len);
        SocketDataDealThread::env->SetByteArrayRegion(jarray,0,len,(jbyte*)getBuffer);
        SocketDataDealThread::env->CallVoidMethod(obj,mid,jarray);
        SocketDataDealThread::env->DeleteLocalRef(jarray);

        }
    }
    LOGI("%s:SocketDataDealThread exit",TAG);
    return NULL;
}

SocketDataDealThread::SocketDataDealThread(int fd,jobject obj1):
        threadId(0), sendLength(0){
    if(pthread_create(&threadId,NULL,SocketDataDealThread::clientThread,NULL) != 0 ){
        LOGI("%s:pthread_create error",TAG);
    }
    LOGI("%s:mSTh->getSocketThreadId():%lu",TAG,(long)threadId);
    SocketDataDealThread::obj = obj1;
    getBuffer = new char[100];
    sendBuffer = new char[100];
    socketFd=fd;
}
SocketDataDealThread::~SocketDataDealThread() {
    delete getBuffer;
    delete sendBuffer;
}
void SocketDataDealThread::sendData(char *buff, int length) {
    LOGI("%s:send data %s,len = %d",TAG,buff,length);
    int len = send(socketFd,buff, length,0);
    if(len<0){
        LOGI("%s:send data error,len = %d",TAG,len);
    }
    wakeUpThread();
}
pthread_t SocketDataDealThread::getSocketThreadId() {
    return threadId;
}
void SocketDataDealThread::wakeUpThread(){
    pthread_mutex_lock(&SocketDataDealThread::mutex);
    // 设置条件为真
    pthread_cond_signal(&SocketDataDealThread::cond);
    pthread_mutex_unlock(&SocketDataDealThread::mutex);
}


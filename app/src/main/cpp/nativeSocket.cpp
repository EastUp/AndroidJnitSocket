#include <stdlib.h>
#include <stdio.h>
#include <jni.h>
#include <assert.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <jni.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include "my_log.h"
#include "StringTools.h"
#include "SocketDataDealThread.h"
#include "ClientSocketDataDealThread.h"

#define HELLO_WORLD_SERVER_PORT    6666
#define LENGTH_OF_LISTEN_QUEUE     20
#define BUFFER_SIZE                1024

extern "C"{

}
static int socketFd=0;
static jobject gObj;
static JNIEnv *gEnv;
static SocketDataDealThread *mSTh;
static void initSocket(JNIEnv* env, jobject thiz,jstring ip,jint port){
    LOGI("%s:initSocket",TAG);
    char *lip = StringTools::jstringTostring(env,ip);
    socketFd = socket(AF_INET,SOCK_STREAM, 0);
    struct timeval timeout = {1,0};
    //设置发送超时
    setsockopt(socketFd,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout,sizeof(struct timeval));
//设置接收超时
    setsockopt(socketFd,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
    //接收缓冲区设置为32K
    int nRecvBuf = 32*1024;
    setsockopt(socketFd,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
    //发送缓冲区设置为32K
    int nSendBuf = 32*1024;
    setsockopt(socketFd,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));
    int nZero=0;
    setsockopt (socketFd,SOL_SOCKET,SO_SNDBUF,(const char *)&nZero,sizeof(nZero));
    ///定义sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(lip);  ///服务器ip
    servaddr.sin_port = htons(port);  ///服务器端口
    ///连接服务器，成功返回0，错误返回-1
    assert(connect(socketFd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == 0);
    gObj = env->NewGlobalRef(thiz);
    mSTh = new SocketDataDealThread(socketFd,gObj);
    LOGI("%s:initSocket,ip = %s,port = %d",TAG,lip,port);
}

static void closeSocket(JNIEnv* env, jobject thiz){
    void *status;
    LOGI("%s:closeSocket",TAG);
    SocketDataDealThread::isShoudExit = true;
    mSTh->wakeUpThread();
    pthread_join(mSTh->getSocketThreadId(), &status);
    delete mSTh;
    env->DeleteGlobalRef(gObj);
    close(socketFd);
}
static void sendData(JNIEnv* env, jobject thiz,jbyteArray buffer){
    jbyte *lb = env->GetByteArrayElements(buffer, NULL);
    int length = env->GetArrayLength(buffer);
    LOGI("%s:sendData,%s,length : %d",TAG,lb,length);
    mSTh->sendData((char *)lb,length);
    env->ReleaseByteArrayElements(buffer,lb,0);
}

static int socketFd2=0;
static int serverSocketFd=0;
static jobject gObj2;
static JNIEnv *gEnv2;
static ClientSocketDataDealThread *mSTh2;
pthread_t threadId = 1;

void * serverThread(void *args) {
    while(1)
    {
        struct sockaddr_in client_addr;
        socklen_t          length = sizeof(client_addr);

        socketFd2 = accept(serverSocketFd, (struct sockaddr*)&client_addr, &length);
        if (socketFd2 < 0)
        {
            printf("Server Accept Failed!\n");
            break;
        }
        LOGI("%s:有客户端连接进入",TAG);
//        char *buffer = new char[100];
//        int len = recv(socketFd2, buffer, sizeof(buffer), 0);
//        LOGI("%s:len=%d",TAG,len);
        gObj = (jobject)(args);
        mSTh2 = new ClientSocketDataDealThread(socketFd2,gObj);
        break;
    }
}


static void initSocket2(JNIEnv* env, jobject thiz){
    // set socket's address information
    struct sockaddr_in   server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);

    // create a stream socket
    serverSocketFd = socket(PF_INET, SOCK_STREAM, 0);
    if (serverSocketFd < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }

    //bind
    if (bind(serverSocketFd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        printf("Server Bind Port: %d Failed!\n", HELLO_WORLD_SERVER_PORT);
        exit(1);
    }

    // listen
    if (listen(serverSocketFd, LENGTH_OF_LISTEN_QUEUE))
    {
        printf("Server Listen Failed!\n");
        exit(1);
    }

//    gObj2 = env->NewGlobalRef(thiz);
//    if(pthread_create(&threadId,NULL,serverThread,gObj2) != 0 ){
//        LOGI("%s:pthread_create error",TAG);
//    }

    while(1)
    {
        struct sockaddr_in client_addr;
        socklen_t          length = sizeof(client_addr);

        socketFd2 = accept(serverSocketFd, (struct sockaddr*)&client_addr, &length);
        if (socketFd2 < 0)
        {
            printf("Server Accept Failed!\n");
            break;
        }

        gObj2 = env->NewGlobalRef(thiz);
        mSTh2 = new ClientSocketDataDealThread(socketFd2,gObj2);
        break;
    }

}



static void closeSocket2(JNIEnv* env, jobject thiz){
    void *status;
    LOGI("%s:closeSocket",TAG);
    ClientSocketDataDealThread::isShoudExit = true;
    mSTh2->wakeUpThread();
    pthread_join(threadId, &status);
    pthread_join(mSTh2->getSocketThreadId(), &status);
    delete mSTh2;
    env->DeleteGlobalRef(gObj2);
    close(socketFd2);
    close(serverSocketFd);
}

static JNINativeMethod gMethods[] = {
        { "initSocket", "(Ljava/lang/String;I)V",(void*) initSocket },
        { "initSocket2", "()V",(void*) initSocket2 },
        { "closeSocket", "()V",(void*) closeSocket },
        { "closeSocket2", "()V",(void*) closeSocket2 },
        { "sendData", "([B)V",(void*) sendData },
};

/*
 * 为某一个类注册本地方法
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
                                 JNINativeMethod* gMethods, int numMethods) {
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

/*
 * 为所有类注册本地方法
 */
static int registerNatives(JNIEnv* env) {
    const char* kClassName = "com/sharenew/androidjnitsocket/MainActivity"; //指定要注册的类
    return registerNativeMethods(env, kClassName, gMethods,
                                 sizeof(gMethods) / sizeof(gMethods[0]));
}

/*
 * System.loadLibrary("lib")时调用
 * 如果成功返回JNI版本, 失败返回-1
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    jint result = -1;
    SocketDataDealThread::javavm = vm;
    ClientSocketDataDealThread::javavm = vm;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    assert(env != NULL);

    if (!registerNatives(env)) { //注册
        return -1;
    }
    //成功
    result = JNI_VERSION_1_4;

    return result;
}
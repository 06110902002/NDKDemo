#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <stack>
#include <stdarg.h>
#include <android/log.h>
#include <VideoFrame.h>
#include <ArrayTest.h>
#include <SortMgr.h>
#include <LinkList.h>
#include <Memory.h>
#include <ObjPool.h>
#include <LruCache.h>
#include <queue/FrameQueue.h>
#include <Handler.h>
#include "LogUtils.h"
#include "utils/SafeQueue.h"
#include "utils/Aircraft.h"
#include "utils/Bomber.h"
#include "utils/BlockQueue.h"
#include "utils/threadsafe_queue.cpp"
#include "StackTest.h"
#include "SmartPoint.h"
#include "NDKStackUtils.h"




pthread_t pthread;//线程对象





template <typename T>
int testTemplateMethod(T a, T b) {

    return a + b;
}

void test(){
    int a = 10;
    char b = 'b';
    int result = testTemplateMethod<int>(a ,b);
    LOGV("泛型函数调用:%d", result);
}


void testTemplateClass(){
    SafeQueue<int> q;

    q.push(6);
    q.push(7);

    LOGV("38-----队列中第一个删除的元素为:  %d",q.pop());
    LOGV("39-----队列中第二个删除的元素为:  %d  size:%d",q.pop(),q.size());
}


jboolean startProConsumer = true;
SafeQueue<char* > base;
void *produce(void *args){
    int i = 0;
    while(startProConsumer){
        int tmp = i ++;

        char str[35];
        sprintf(str,"商品%d",tmp);
        base.push(str);
        LOGV("65-----生产者生产 :%d  号商品,仓库中有:%d 个商品",tmp,base.size());
       sleep(0.04);
    }
    LOGV("76-------生产者 停止工作");
    pthread_exit(nullptr);
}

void *consumer(void *args){
    while(startProConsumer){
        char* tmp = base.pop();

        LOGV("65-----消费者消费 :%s ,仓库剩余：%d",tmp,base.size());
        sleep(0.01);
    }
    LOGV("86-------消费者 停止工作");
    pthread_exit(nullptr);
}

void testProduceAndComsuer(){

    startProConsumer = true;
    //创建生产者线程
    // 创建一个线程id
    pthread_t produceThread;
    pthread_create(&produceThread, nullptr, produce, (void*)"produce");


    pthread_t comsumerThread;
    pthread_create(&comsumerThread, nullptr, consumer, (void*)"consumer");

}

JavaVM *gvm;
jobject gCallBackObj;
jmethodID gCallBackMid;
jmethodID onJavaByteId;

jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved){
    LOGV("317--------JNI_OnLoad");
    gvm = vm;
    test();
    testTemplateClass();
    //testProduceAndComsuer();
    return JNI_VERSION_1_6;
}



extern "C" JNIEXPORT jstring JNICALL
Java_come_live_ndkdemo_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    LOGI("26------hello");
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT jstring JNICALL
Java_come_live_ndkdemo_NativeTest_testCallNativeMethod(JNIEnv *env, jobject thiz) {
    // TODO: implement testCallNativeMethod()

    //先找类对象
    jclass nativeTest = env->FindClass("come/live/ndkdemo/NativeTest");
    if( nativeTest == NULL){
        LOGV("找不到nativeTest类对象");
        return NULL;
    }

    /**
     * 找到类对象，先初始化，获取类对象实例,使用默认的构造方法
     */
    jmethodID constructor = env->GetMethodID(nativeTest, "<init>", "()V");
    if (NULL == constructor) {
        LOGW("can't constructor nativeTest");
        return nullptr;
    }
    /**
     * 构造nativeTestObject 实例对象
     */
    jobject nativeTestObject = env->NewObject(nativeTest, constructor);
    if (NULL == nativeTestObject) {
        LOGW("can't new nativeTestObject");
        return NULL;
    }

    /**
     * 查找要调用的方法
     */
    jmethodID nativeCallJavaMethod = env->GetMethodID(nativeTest,"nativeCallJavaMethod","(ILjava/lang/String;)V");
    if (nativeCallJavaMethod == NULL) {
        LOGV("nativeCallJavaMethod 未找到，直接释放类对象与实例对象");
        env->DeleteLocalRef(nativeTest);
        env->DeleteLocalRef(nativeTestObject);
        return NULL;
    }
    /**
     * 构造参数
     */
    jstring jstrMSG =env->NewStringUTF( "Hi,I'm From C");
    env->CallVoidMethod(nativeTestObject,nativeCallJavaMethod,3,jstrMSG);

    /**
     * 调用java 静态方法
     */
    jmethodID nativeCallJavaStaticMethod = env->GetStaticMethodID(nativeTest,"nativeCallJavaStaticMethod","()V");
    env->CallStaticVoidMethod(nativeTest,nativeCallJavaStaticMethod);

    /**
     * 调用java 带返回值的方法
     */
    jmethodID getStringForJava = env->GetMethodID(nativeTest,"getStringForJava","()Ljava/lang/String;");
    if(getStringForJava == NULL){
        LOGV("getStringForJava 未找到，直接释放类对象与实例对象");
        env->DeleteLocalRef(nativeTest);
        env->DeleteLocalRef(nativeTestObject);
        return NULL;
    }

    jstring result = static_cast<jstring>(env->CallObjectMethod(nativeTestObject, getStringForJava));
    /**
     * jni 不能直接使用java 对象，需要将jstring转换为c 对象
     */
    const char *resultChar = env->GetStringUTFChars(result, NULL);
    LOGV("调用java 方法 getStringForJava 获取 的返回值为:%s",resultChar);

    /**
     * 获取对象实例属性
     */
    jfieldID ageId = env->GetFieldID(nativeTest,"age","I");
    jint age = env->GetIntField(nativeTestObject,ageId);
    LOGV("获取对象NativeTest 实例属性 age 值为：%d",age);


    /**--------------------------------回调byte[]函数---------------------------------------------*/
    /**
     * 查找要调用的方法
     */
    jmethodID onByte = env->GetMethodID(nativeTest,"onByte","([BI)V");
    if (onByte == NULL) {
        LOGV("nativeCallJavaMethod 未找到，直接释放类对象与实例对象");
        env->DeleteLocalRef(nativeTest);
        env->DeleteLocalRef(nativeTestObject);
        return NULL;
    }
    /**
     * 构造参数
     */
    char *m_body = "0123456789abc&*()";
    int len = 9;
    char *tmp = new char[len];
    memset(tmp,0,len);
    memcpy(tmp,m_body + 2,len);

    //转换
    jbyteArray jbArray = env->NewByteArray(len);
    env->SetByteArrayRegion(jbArray, 0, len, (jbyte *) tmp);
    env->CallVoidMethod(nativeTestObject, onByte, jbArray, len);
    delete[] tmp;
    env->DeleteLocalRef(jbArray);

    /**--------------------------------end---------------------------------------------*/


    /**
     * 用完之后，释放局部引用
     */
    env->DeleteLocalRef(result);
    env->DeleteLocalRef(jstrMSG);
    env->DeleteLocalRef(nativeTest);
    env->DeleteLocalRef(nativeTestObject);

    return NULL;

}

extern "C"
JNIEXPORT jstring JNICALL
Java_come_live_ndkdemo_NativeTest_callNativeArray(JNIEnv *env, jobject thiz) {
    // TODO: implement callNativeArray()
    LOGI("40------测试使用native数组");
    //新建一个jintArray对象
    jintArray jint_arr_temp = env->NewIntArray(21);

    //获取jint_arr_temp对象的指针
    jint *int_arr_temp = env->GetIntArrayElements(jint_arr_temp, NULL);

    //获取数组的长度
    jint len = env->GetArrayLength(jint_arr_temp);
    LOGI("数组长度：%d", len);


    //计数
    jint count = 0;
    //偶数位存入到int_arr_temp内存中
    for (jsize j = 0; j < 21; j++) {

        if (j % 2 == 0) {
            int_arr_temp[count++] = j;
        }
    }
    for (jsize i = 0; i < 21; i++) {
        LOGI("数组元素：%d", int_arr_temp[i]);
    }

    env->ReleaseIntArrayElements(jint_arr_temp,0,2);

    return env->NewStringUTF("hello.c_str()");

}



/**-----------------测试函数调用栈-------------*/
void fun1() {
    NDKStackUtils ndkStackUtils;
    ndkStackUtils.callstackLogcat(ANDROID_LOG_DEBUG,"testNDKDebug");
}

void fun2() {
    fun1();
}

void testPrintNativeCallStack() {
    fun2();
}
/**-----------------测试函数调用栈-------------*/




/**
 * 原生代码可以用GetArrayElements函数获取指向数组元素的直接指针。
 * 第三个参数和我们在上篇博文中提到的一样，它是一个可选参数，该可选参数的名称为isCopy，
 * 让调用者确定返回的C字符串地址指向副本还是指向堆中的固定对象。
 * 因为可以像普通的C数组一样访问和处理数组元素，因此JNI没有提供访问和处理元素的方法，
 * JNI要求原生代码用完这些指针必须立刻释放，否则会出现内存溢出问题，原生代码可以使用JNI提供的ReleaseArrayElements函数来释放GetArrayElements函数返回的C数组
 */
extern "C"
JNIEXPORT jint JNICALL
Java_come_live_ndkdemo_NativeTest_sumArray(JNIEnv *env, jobject thiz, jintArray javaArray) {
    // TODO: implement sumArray()

    jboolean isCopy;
    jint result = 0;

    jsize length = env->GetArrayLength(javaArray);

    /**
     * jni 不能直接操作操作java数组，需要转换
     * 将Java数组区复制到C数组中，这里有通过指针访问原生的数组，需要在不用的时候做释放
     */
    jint *nativeDirectArray = env->GetIntArrayElements(javaArray, &isCopy);

    int i = 0;
    for (i = 0; i < length; i++) {
        result += nativeDirectArray[i];
    }
    /**
     * 对原生的数组使用之后都需要使用释放，防止内存泄露
     * 最后的模式：
     * 0：将内容复制回来并释放原生数组
     * 1 = JNI_COMMIT：将内容复制回来但是不释放原生数组，一般用于周期性的更新一个Java数组。
     * 2 = JNI_ABORT ：释放原生数组但是不将内容复制回来。
     */
    env->ReleaseIntArrayElements(javaArray, nativeDirectArray, 0);


    //打印函数调用堆栈
    testPrintNativeCallStack();

    return result;

}



extern "C" JNIEXPORT jintArray JNICALL Java_come_live_ndkdemo_NativeTest_getNativeArray(JNIEnv *env, jobject thiz) {
    // TODO: implement getNativeArray()

    /**
     * 定义一个native 数组，jni 可直接操作
     */
    jint nativeArr[10] = {0,1,2,3,4,5,6,7,8,9};

    /**
     * 通过当前线程中 java在jni 环境的代表 env实例化一个数组
     * 凡是java 对象，都需要通过env 操作，因为java 在native 中的环境代表都是env
     * 且env 是当前线程独有
     * 新建长度len数组 申请内存
     */
    jintArray javaArray = env->NewIntArray(10);

    /**
     * 将native数组nativeArr 中的内容复制给javaArray
     */
    env->SetIntArrayRegion(javaArray, 0, 10, nativeArr);

    /**
     * 如果是在JNI函数内通过NewStringUTF、NewXXXArray或NewObject创建的java对象无论是否需要返回java层，
     * 都不需要手动释放，jvm会自动管理。但是如果是通过 AttachCurrentThread 创建的 JNIEnv 去New的对象，
     * 必须通过 DeleteLocalRef 方式及时删除，因为在线程销毁之前，创建的对象无法自动回收
     * 参考链接：https://www.jianshu.com/p/5cde114159d4
     */
    //env->ReleaseIntArrayElements(javaArray, nativeArr, 0);

    return javaArray;
}


extern "C"
JNIEXPORT jintArray JNICALL
Java_come_live_ndkdemo_NativeTest_createArrayMethod(JNIEnv *env, jobject thiz, jint len) {
    // TODO: implement createArrayMethod()

    //1.新建长度len数组 申请内存
    jintArray jarr = env->NewIntArray(len);

    //2.通过 jni获取 jarr 数组在jni 环境中的指针，因为jni 只能操作jni环境的对象
    jint *arr = env->GetIntArrayElements(jarr, NULL);
    //3.赋值
    int i = 0;
    for(; i < len; i++){
        arr[i] = i;
    }
    //4.释放资源
    /**
    * 对原生的数组使用之后都需要使用释放，防止内存泄露
    * 最后的模式：
    * 0：将内容复制回来并释放原生数组
    * 1 = JNI_COMMIT：将内容复制回来但是不释放原生数组，一般用于周期性的更新一个Java数组。
    * 2 = JNI_ABORT ：释放原生数组但是不将内容复制回来。
    */
    env->ReleaseIntArrayElements(jarr, arr, 0);
    //5.返回数组
    return jarr;
}




extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_passJavaObj2Native(JNIEnv *env, jobject thiz, jobject person) {
    // TODO: implement passJavaObj2Native()

    //先找类对象
    jclass personClz = env->GetObjectClass(person);
    if( personClz == NULL){
        LOGV("找不到personClz类对象");
        return ;
    }


    jfieldID nameId  = env->GetFieldID(personClz , "name", "Ljava/lang/String;");
    jstring nameStr = static_cast<jstring>(env->GetObjectField(person, nameId));
    const char * c_name = env->GetStringUTFChars(nameStr ,NULL);//转换成 char *
    LOGV("235-------获取java 传过来的person 对象 name属性值为：%s",c_name);
    env->ReleaseStringUTFChars(nameStr,c_name);


    jmethodID getNameId = env->GetMethodID(personClz,"getName","()Ljava/lang/String;");
    jstring name = static_cast<jstring>(env->CallObjectMethod(person, getNameId));
    const char * c_name2 = env->GetStringUTFChars(name ,NULL);//转换成 char *
    LOGV("247------获取java 传过来的person 对象 name属性值为：%s",c_name2);

    env->ReleaseStringUTFChars(name,c_name2);
    env->DeleteLocalRef(personClz);


}extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_nativeCallJavaMethod(JNIEnv *env, jobject thiz, jint age, jstring name) {
    // TODO: implement nativeCallJavaMethod()



}extern "C"
JNIEXPORT jobject JNICALL
Java_come_live_ndkdemo_NativeTest_getListFromNative(JNIEnv *env, jobject thiz) {
    // TODO: implement getListFromNative()
    //获得ArrayList类引用
    jclass list_cls = env->FindClass("java/util/ArrayList");
    if(list_cls == NULL){
        LOGV("查找集合java/util/ArrayList失败");
        return NULL;
    }
    /**
     * 查找构造方法
     */
    jmethodID list_contruct = env->GetMethodID(list_cls,"<init>","()V");
    if(list_contruct == NULL){
        LOGV("构造方法查找失败");
        return NULL;
    }
    /**
     * 创建ArrayList实例对象
     */
    jobject list_obj = env->NewObject(list_cls,list_contruct);
    jmethodID list_add = env->GetMethodID(list_cls,"add","(Ljava/lang/Object;)Z");


    jclass personClz = env->FindClass("come/live/ndkdemo/Person");
    jmethodID person_construct = env->GetMethodID(personClz,"<init>","()V");
    jmethodID person_setAge = env->GetMethodID(personClz,"setAge","(I)V");
    jmethodID person_setName = env->GetMethodID(personClz,"setName", "(Ljava/lang/String;)V");


    for (jint i = 0; i < 5; i++) {

        jstring str = env->NewStringUTF("Native");
        //通过调用该对象的构造函数来new 一个 Student实例
        jobject tmp_person_obj = env->NewObject(personClz, person_construct);

        env->CallVoidMethod(tmp_person_obj, person_setAge, i);
        env->CallVoidMethod(tmp_person_obj, person_setName, str);
        /**
         * 将构造出来的对象添加集合中
         */
        env->CallBooleanMethod(list_obj, list_add, tmp_person_obj);


        env->DeleteLocalRef(tmp_person_obj);
    }

    env->DeleteLocalRef(personClz);

    return list_obj;
}







/**
 * 相当于java中线程的run方法
 * @return
 */
void *writeFile(void *args) {
    // 随机字符串写入
    FILE *file;
//    if ((file = fopen("/sdcard/thread_cb", "a+")) == nullptr) {
//        LOGV("fopen filed");
//        return nullptr;
//    }
//    for (int i = 0; i < 10; ++i) {
//        fprintf(file, "test %d\n", i);
//    }
//    fflush(file);
//    fclose(file);
    LOGV("330------file write done");

    JNIEnv *env = nullptr;
    // 将当前线程添加到Java虚拟机上，返回一个属于当前线程的JNIEnv指针env
    if (gvm->AttachCurrentThread(&env, nullptr) == 0) {
        jstring jstr = env->NewStringUTF("write success");
        // 回调到java层
        env->CallVoidMethod(gCallBackObj, gCallBackMid, jstr);





        /**--------------------------------回调byte[]函数---------------------------------------------*/
        while(true) {
            /**
             * 构造参数
             */
            char *m_body = "0123456789abc&*()abcdfghtyui90-=7654这是一条来自native的数据用来测试内存是否泄露的56789abc&*()abcdfghtyui90-=7654这是一条来自native的数据用来测试内存是否泄露的";
            int len = 40;
            char *tmp = new char[len];
            memset(tmp, 0, len);
            memcpy(tmp, m_body + 4, len);
            //转换
            jbyteArray jbArray = env->NewByteArray(len);
            env->SetByteArrayRegion(jbArray, 0, len, (jbyte *) tmp);
            env->CallVoidMethod(gCallBackObj, onJavaByteId, jbArray, len);
            env->DeleteLocalRef(jbArray);
            delete[] tmp;
            sleep(1);
        }


        /**--------------------------------end---------------------------------------------*/





        // 删除jni中全局引用
        env->DeleteGlobalRef(gCallBackObj);
        // 从Java虚拟机上分离当前线程
        gvm->DetachCurrentThread();
    }
    return nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_nativeInThreadCallBack(JNIEnv *env, jobject thiz, jobject listener) {
    // TODO: implement nativeInThreadCallBack()

    // 创建一个jni中的全局引用
    gCallBackObj = env->NewGlobalRef(listener);
    jclass cls = env->GetObjectClass(listener);
    gCallBackMid = env->GetMethodID(cls, "onSuccess", "(Ljava/lang/String;)V");
    onJavaByteId = env->GetMethodID(cls, "onByte","([BI)V");


    // 创建一个线程
    pthread_t pthread;
    jint ret = pthread_create(&pthread, nullptr, writeFile, nullptr);
    LOGV("360------pthread_create ret=%d", ret);
    env->DeleteLocalRef(cls);
}

void testVirture(Aircraft* aircraft){
    if(aircraft){
        aircraft->refuel();
        aircraft->fly();
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_testNativeVirtureMethod(JNIEnv *env, jobject thiz) {
    // TODO: implement testNativeVirtureMethod()

    Bomber* bomber = new Bomber();
    testVirture(bomber);
    bomber->attack();
}

/*
 * 求最大值
 * 返回值是int类型，返回两个整数中较大的一个
 */
int max(int a, int b) {
    return a > b ? a : b;
}

/*
 * 求最小值
 * 返回值是int类型，返回两个整数中较小的一个
 */
int min(int a, int b) {
    return a < b ? a : b;
}

/**
 * 函数指针当作参数传递
 * @param e
 * @param d
 * @param f
 */
void funcPointAsParmas(int e, int d, int(*f)(int a, int b)){
    LOGV("434-----------函数指针当作参数传递:%d",f(e, d));
}



/*
 * 指针函数的定义
 * 返回值是指针类型int *
 */
int *f(int a, int b) {

    return reinterpret_cast<int *>(a + b);
}


/**
 * 函数指针可以 把函数作为参数传入另一个函数
 */
extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_testNativeFucPoint(JNIEnv *env, jobject thiz) {
    // TODO: implement testNativeFucPoint()
    Bomber *bomber = new Bomber();
    bomber->p = max;
    int tmpMax = (bomber->p)(2, 3);
    LOGV("函数指针求得的最大值：%d", tmpMax);
    bomber->p = min;
    int tmpMin = (bomber->p)(-9, 0);
    LOGV("函数指针求得的最小值：%d", tmpMin);

    funcPointAsParmas(2,12,max);

}

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_testsNativePointFuc(JNIEnv *env, jobject thiz) {
    // TODO: implement testsNativePointFuc()

    int *p1 = f(1, 2);
    LOGV("指针函数获取到的值为：%d", p1);
}


uint8_t *int2Bytes(int i) {

    unsigned char buf[4];
    buf[0] = (unsigned char) i;
    buf[1] = i >> 8;
    buf[2] = i >> 16;
    buf[3] = i >> 24;
    return buf;

}


extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_dataStructTest(JNIEnv *env, jobject thiz) {
    // TODO: implement dataStructTest()


    auto* blockQueue = new BlockQueue<int>();
    auto* a = new Node<int>();
    a->data = 2;
    a->next = nullptr;
    blockQueue->add(a);

    auto* b = new Node<int>();
    b->data = 3;
    b->next = nullptr;
    blockQueue->add(b);

    Node<int>* tmp = blockQueue->pop();
    LOGV("543------获取链表头元素:%d",tmp->data);

    Node<int>* p = blockQueue->head;
    while(p){
        LOGV("53------:%d",p->data);
        p = p->next;
    }


    vector<int> vector;

    for (int i = 1; i <= 10; i++){
        vector.push_back(i);
    }
    int s = vector.front();
    vector.erase(vector.begin());
    LOGV("553--------:%d",s);
    for (auto ir = vector.crbegin(); ir != vector.crend(); ++ir){
        LOGV("555----------:%d",*ir);
    }


    SafeQueue<VideoFrame*> frameQueue;
    threadsafe_queue<VideoFrame*> frameQueue2;

    for(int i = 0; i < 10; i ++){
        uint8_t* buff = int2Bytes(i * 10);
        VideoFrame* frame = new VideoFrame(buff,23,100 + i);
        frameQueue.push(frame);

        frameQueue2.push(frame);
    }

   /* while(frameQueue.size() > 0){
        VideoFrame* head = frameQueue.pop();
        LOGV(" 589------测试type: %d  length is: %d",head->type,head->length);
    }*/

    LOGV("596-------frameQueue2是否为空：%d   元素个数:%d",frameQueue2.empty(),frameQueue2.size());
    while(!frameQueue2.empty()){
        VideoFrame* head;
        frameQueue2.wait_and_pop(head);
        LOGV(" 598------测试type: %d  length is: %d",head->type,head->length);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_produceConsumer(JNIEnv *env, jobject thiz) {
    // TODO: implement produceConsumer()

    testProduceAndComsuer();
}

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_stopProduceConsumer(JNIEnv *env, jobject thiz) {
    // TODO: implement stopProduceConsumer()
    startProConsumer = false;
}

StackTest* stackTest;


extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_testStack(JNIEnv *env, jobject thiz) {
    // TODO: implement testStack()
    if(stackTest == nullptr){
        stackTest = new StackTest();
    }
    stackTest->testBaseUse();

}



extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_stackMoniDeQueue(JNIEnv *env, jobject thiz) {
    // TODO: implement stackMoniDeQueue()
    if(stackTest == nullptr){
        stackTest = new StackTest();
    }
    int head = stackTest->moniDeQueue();
    LOGV("632------出队元素:%d",head);
}

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_stackMoniEnqueue(JNIEnv *env, jobject thiz, jint e) {
    if(stackTest == nullptr){
        stackTest = new StackTest();
    }
    stackTest->moniEnqueue((int)e);
}

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_computer(JNIEnv *env, jobject thiz, jstring s) {
    // TODO: implement computer()
    if(stackTest == nullptr){
        stackTest = new StackTest();
    }
    const char* s2 = env->GetStringUTFChars(s, 0);

    float result = stackTest->computer((char*)s2);
    LOGV("661------式子：%s  计算结果为:%f",s2,result);
}

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_mid2Pre(JNIEnv *env, jobject thiz, jstring s) {
    // TODO: implement mid2Pre()
    if(stackTest == nullptr){
        stackTest = new StackTest();
    }
    const char* s2 = env->GetStringUTFChars(s, 0);
    char* result = stackTest->midSufix2PreSufix((char*)s2);
    LOGV("673------中缀式子：%s  转换为前缀:%s",s2,result);
}

ArrayTest* arrayTest;
extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_array(JNIEnv *env, jobject thiz) {
    // TODO: implement array()
    if(arrayTest == nullptr){
        arrayTest = new ArrayTest();
    }
    arrayTest->test();

}extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_migong(JNIEnv *env, jobject thiz) {
    // TODO: implement migong()

    if(arrayTest == nullptr){
        arrayTest = new ArrayTest();
    }
    arrayTest->queryPath();
}

Person* person;
Person* person2;
extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_pointPassValue(JNIEnv *env, jobject thiz) {
    // TODO: implement pointPassValue()
    if(arrayTest == nullptr){
        arrayTest = new ArrayTest();
    }
    person = new Person(21);
    arrayTest->pointPassValue(person);
    LOGV("709-----指针传值修改之后的值：%d",person->age);

}

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_refPassValue(JNIEnv *env, jobject thiz) {
    // TODO: implement refPassValue()
    if(arrayTest == nullptr){
        arrayTest = new ArrayTest();
    }
    person2 = new Person(223);
    arrayTest->refPassValue(*person2);
    int a = 1;
    int c = (a ++) + (++a) + (a++)+(++a);
    LOGV("722-----引用传值修改之后的值：%d  c = %d",person2->age,c);



    char  ch[4] = {'a','b','c','\0'};

    //指向常量的指针：指向的是一个常量，"abcdefg" 为字面常量，分配在内存的常量区，内容不能改变
    //所以 str 不能改变它指向内存地址的内容，但是可以改变它的指向，比如可以让它指向另一个内存地址
    const char *str = "abcdefg";
    str = ch;
    LOGV("727---------:%s",str);

    //常量指针，本身还是一个指针，但是这个指针是一个常量，不能重新改变它的指向，但是
    //可以改变当前它指向内存地址的内容如下：
    char s1[4] = {'a','b','c','\0'};
    char* const p = s1;
    LOGV("738-------常量指针初始化的地址：%p  内容为:%s",p,p);
    char tmp = 'c';
    //p = &tmp;  因为重新改变了它的指向  所以编译报错
    s1[0] = tmp;
    LOGV("741-----常量改变其指向内存地址的内容之后的地址：%p  值为 ：%s",p,p);

}
SortMgr* sortMgr;
extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_quickSort(JNIEnv *env, jobject thiz) {
    // TODO: implement quickSort()
    if(sortMgr == nullptr) {
        sortMgr = new SortMgr();
    }
    int a[9] = {9,3,5,1,7,8,4,6,2};
    int* b = a;
    //LOGV("757-------:a[0]:%d",b[8]);
    sortMgr->quickSort(a,0,8);
    for(int i =0; i < 9; i ++ ){
        LOGV("761--------:%d",a[i]);
    }
}

LinkList* linkList;


extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_linkedList(JNIEnv *env, jobject thiz) {
    // TODO: implement linkedList()
    if(linkList == nullptr) {
        linkList = new LinkList();
    }
    int a[] = {2,1,4,5,3,6,7,9,8,0};
    LNode* head = linkList->create(a,10);
    LNode* reverserNode = linkList->reverseList(head);

}

Memory* memory;

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_memoryTest(JNIEnv *env, jobject thiz) {
    // TODO: implement memoryTest()

    if(memory == nullptr) {
        memory = new Memory();
    }
    delete memory;
    memory = nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_lambda(JNIEnv *env, jobject thiz) {
    // TODO: implement lambda()
    if(memory == nullptr) {
        memory = new Memory();
    }
    memory->testLambda();
}

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_smartPoint(JNIEnv *env, jobject thiz) {
    // TODO: implement smartPoint()

    SmartPoint<Computer> sp(new Computer(2300));
    sp->test();

}

ObjPool<Car>* objPool;
Car* tmpCar;

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_getObjFromPool(JNIEnv *env, jobject thiz) {
    // TODO: implement getObjFromPool()
    if(objPool == nullptr){
        objPool = new ObjPool<Car>(10);
    }
    tmpCar = objPool->getObjFromPool();
    if(tmpCar != nullptr){
        LOGV("830-----从对象池取对象 车子价格:%d  颜色:%s",tmpCar->m_price,tmpCar->m_color);
    }


}

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_recycObj2Pool(JNIEnv *env, jobject thiz) {
    // TODO: implement recycObj2Pool()
    if(objPool == nullptr){
        objPool = new ObjPool<Car>(10);
    }
    objPool->recycObj2Pool(*tmpCar);
}


LruCache<char*, char*>* lrucache;
int index = 0;

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_writeCache(JNIEnv *env, jobject thiz) {
    // TODO: implement writeCache()
    if(lrucache == nullptr) {
        lrucache = new LruCache<char*, char*>(3);
    }
    index ++;
    char* key = "A";
    char* value = "this is test data";
    lrucache->updateCache(key,value);
}


FrameQueue* frameQueue;

extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_readCache(JNIEnv *env, jobject thiz) {
    // TODO: implement readCache()
    if(lrucache != nullptr) {
        char* key = "A";
        char* value = lrucache->getData(key);
        lrucache->print();
        LOGV("867----LRU 中key = %s  获取其值 value = %s：",key,value);
    }

    if(!frameQueue) {
        frameQueue = new FrameQueue(5,true);
    }
    Frame* vp = frameQueue->peekWritable();

    if(vp) {
        vp->uploaded = 0;
        vp->width = 480;
        vp->height = 640;
        vp->format = 23;
        vp->pts = 1920;
        vp->duration = 40;
        frameQueue->pushFrame();
    }


    Frame* vp2 = frameQueue->peekWritable();
    if(vp2) {
        vp2->uploaded = 0;
        vp2->width = 580;
        vp2->height = 40;
        vp2->format = 273;
        vp2->pts = 190;
        vp2->duration = 70;
        frameQueue->pushFrame();
    }

    Frame* vp3 = frameQueue->peekWritable();
    if(vp3) {
        vp3->uploaded = 0;
        vp3->width = 580;
        vp3->height = 40;
        vp3->format = 273;
        vp3->pts = 190;
        vp3->duration = 70;
        frameQueue->pushFrame();
    }

}


extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_delCache(JNIEnv *env, jobject thiz, jstring key) {
    // TODO: implement delCache()
    const char* cacheKey = env->GetStringUTFChars(key, 0);
    LOGV("883------收到待删除的key = %s",cacheKey);
    if(lrucache != nullptr) {
        char* key2 = "A";
        lrucache->remove(key2);
    }

    if(frameQueue) {
       Frame* lastFrame = frameQueue->lastFrame();
        Frame* nextFrame = frameQueue->nextFrame();
        frameQueue->popFrame();
        Frame* curFrame = frameQueue->currentFrame();
       LOGI("977-------lastFrame = %d   curFrame = %d   nextFrame = %d",lastFrame->width,curFrame->width,nextFrame->width);
    }
}




#include "JavaObjectMap.hpp"
using namespace QuarameraJNI;
Bomber* bomber;

extern "C"
JNIEXPORT jlong JNICALL
Java_come_live_ndkdemo_NativeTest_initNativePtr(JNIEnv *env, jobject thiz) {
    // TODO: implement initNativePtr()
    if (bomber == nullptr) {
        bomber = new Bomber();
    }
    return reinterpret_cast<jlong>(bomber);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_come_live_ndkdemo_NativeTest_bindNativePtr(JNIEnv *env, jobject thiz, NativeId<Aircraft> native_ptr) {
    // TODO: implement bindNativePtr()

    //将传进来的指针址 赋值给 aircraft  再返回给java 层
    Aircraft* aircraft = native_ptr.p;
    return reinterpret_cast<jlong>(aircraft);
}
extern "C"
JNIEXPORT jint JNICALL
Java_come_live_ndkdemo_NativeTest_exeNativeWithNativePtr(JNIEnv *env, jobject thiz, NativeId<Aircraft> native_ptr) {
    // TODO: implement exeNativeWithNativePtr()
    native_ptr.p->fly();

    return 1;
}

/**------------------------测试消息队列------------------------*/
MessageQueue* mMsgQueue;
jboolean startMessageQueue = static_cast<jboolean>(true);
Handler* mHandler;
void *messageLoop(void *args){
    while(startMessageQueue) {
        AVMessage msg;
        int retval = mHandler->msg_queue_get(mMsgQueue, &msg, 1);
        if (retval < 0) {
            LOGI("1118-------收到消息报错，退出循环 这种情况可以抛出异常");
            break;
        } else {
            LOGI("1121-------收到 %d 消息",msg.what);
            switch(msg.what) {
                case 20220715:
                   // mHandler->msg_free_res(&msg);
                    break;
            }

        }
        LOGI("1127-------消息loop 正在工作");
    }
    LOGI("1130-------消息loop 停止工作");
    if (mHandler && mMsgQueue) {
        mHandler->msg_queue_destroy(mMsgQueue);
        delete mMsgQueue;
        mMsgQueue = nullptr;
        delete mHandler;
        mHandler = nullptr;
    }
    pthread_exit(nullptr);
}
extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_createNativeMsgQueue(JNIEnv *env, jobject thiz) {
    // TODO: implement createNativeMsgQueue()
    mMsgQueue = new MessageQueue();
    mHandler = new Handler();
    mHandler->msg_queue_init(mMsgQueue);
    mHandler->msg_queue_start(mMsgQueue);

    pthread_t msg_loop;
    pthread_create(&msg_loop, nullptr, messageLoop, (void*)"messageLoop");
}
extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_sendNativeMsg(JNIEnv *env, jobject thiz) {
    // TODO: implement sendNativeMsg()
    LOGI("1125-------发送消息  ");
    for (int i = 0; i < 50; i++) {
        mHandler->msg_queue_put_simple1(mMsgQueue,20220715);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_come_live_ndkdemo_NativeTest_destoryQueue(JNIEnv *env, jobject thiz) {
    // TODO: implement destoryQueue()
    startMessageQueue = static_cast<jboolean>(false);
    LOGI("1153-------销毁队列 startMessageQueue = %d",startMessageQueue);
    mHandler->msg_queue_abort(mMsgQueue);

}
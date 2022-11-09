package come.live.ndkdemo;

import java.util.ArrayList;

import android.nfc.Tag;
import android.util.Log;
import android.view.Surface;

/**
 * author         hengyang.lxb
 * date           2021/01/07
 * Version:       1.0
 * Description:
 */
public class NativeTest {

    public int age = 23;


    private static final String TAG = "Native";

    /**
     * java 调用native方法
     * @return
     */
    public native String testCallNativeMethod();

    /**
     * native 中调用java 方法
     * @param age
     * @param name
     */
    public void nativeCallJavaMethod(int age,String name){
        Log.v(TAG,"我是java 方法 jni 传递来的参数 age:"+age+ " name:" + name);

    }

    /**
     * call rom jni
     * @param data
     * @param length
     */
    public void onByte(byte[] data,int length){
        String msg = new String(data);
        LogUtils.v("这是来自native 的数据:" + msg);

    }

    /**
     * jni 中调用java 静态方法
     */
    public static void nativeCallJavaStaticMethod(){
        Log.v(TAG,"我是java 的静态方法");
    }

    /**
     * native 调用java 方法并获取返回值
     * @return
     */
    public String getStringForJava() {
        return "string from method in java";
    }


    /**
     * 在native 中计算java数组的和并返回
     * @return 返回数组元素和
     */
    public native int sumArray(int[] javaArray);

    /**
     * 获取原生数组
     * @return 原生数组
     */
    public native int[] getNativeArray();

    /**
     * 创建指定长度数组
     * @param len
     * @return
     */
    public native int[] createArrayMethod(int len);


    /**
     * 传递jave 对象给native
     * @param person
     */
    public native void passJavaObj2Native(Person person);

    /**
     * 获取native 返回的集合对象
     * @return
     */
    public native ArrayList<Person> getListFromNative();

    /**
     * java回调接口 INativeThreadListener.java
     */
    public interface INativeThreadListener {

        void onSuccess(String msg);

        void onByte(byte[] data,int length);
    }

    /**
     * 调用native 的线程,并将结果回调到java 层，通过jave回调实现
     */
    public native void nativeInThreadCallBack(INativeThreadListener listener);

    /**
     * 测试jni 中使用C++ 虚函数
     */
    public native void testNativeVirtureMethod();

    /**
     * C++函数指针
     */
    public native void testNativeFucPoint();

    /**
     * C++指针函数
     */
    public native void testsNativePointFuc();


    public native void dataStructTest();

    public native void produceConsumer();

    public native void stopProduceConsumer();

    public native void testReadWriteLock();

    public native void stopReadWriteLock();

    public native void oneByOnePrint();

    public native void stopOneByOne();

    public native void testThreadCpp();

    public native void testStack();

    public native void stackMoniEnqueue(int e);

    public native void stackMoniDeQueue();

    public native void computer(String s);

    /**
     * 中缀表达式转换为前缀表达式
     * @param s
     */
    public native void mid2Pre(String s);

    public native void array();

    public native void migong();

    public native void pointPassValue();

    public native void refPassValue();

    public native void quickSort();

    public native void linkedList();

    public native void memoryTest();

    public native void lambda();

    public native void smartPoint();

    public native void getObjFromPool();

    public native void recycObj2Pool();


    public native void writeCache();
    public native void readCache();
    public native void delCache(String key);

    /**
     * 初始化 native 的指针对象
     *
     * @return  初始化成功的native 地址
     */
    public native long initNativePtr();


    /**
     * java 与native 通过指针进行互调的方式
     * @param nativePtr  native指针
     * @return  返回给java 的指针地址
     */
    public native long bindNativePtr(long nativePtr);

    /**
     * java 通过native 指针  进行native 对象的操作
     *
     * @param nativePtr native 指针地址
     * @return
     */
    public native int exeNativeWithNativePtr(long nativePtr);

    /**
     * 创建底层消息队列
     *
     */
    public native void createNativeMsgQueue();

    public native void destoryQueue();

    public native void sendNativeMsg();

    public native void testMap();

    public native void testRValueRef();

    public native void mediaCodecStart(String path, Surface surface);

    public native void mediaCodecStop();

}

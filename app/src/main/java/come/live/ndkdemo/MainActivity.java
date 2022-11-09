package come.live.ndkdemo;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import android.app.Activity;
import android.content.Intent;
import android.inputmethodservice.Keyboard;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.Spinner;
import android.widget.Toast;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import come.live.ndkdemo.NativeTest.INativeThreadListener;
import come.live.ndkdemo.cache.CacheActivity;
import come.live.player.MicroPlayer;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private NativeTest nativeTest;
    private String[] tt;
    private MicroPlayer microPlayer;
    private GameEditText mEditText;
     String lastText;
     private FrameLayout mLayout;
    SurfaceView surfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        nativeTest = new NativeTest();
        microPlayer = new MicroPlayer();
        initView();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    private void initView() {
        findViewById(R.id.btn_test_array).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                sumArrayByNative();
                getArrayFromNative();
                getArrayFromNative(23);

            }
        });
        findViewById(R.id.btn_pass_obj).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                passObj2Native();
                jniCallJavaMethod();

            }
        });
        findViewById(R.id.btn_get_obj).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                getListObjFromNative();
            }
        });

        findViewById(R.id.btn_native_thread).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.nativeInThreadCallBack(new INativeThreadListener() {
                    @Override
                    public void onSuccess(String msg) {
                        if(Looper.getMainLooper() == Looper.myLooper()) {
                            Toast.makeText(MainActivity.this,"成功在的ui线程更新",Toast.LENGTH_SHORT).show();
                        } else {
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    Toast.makeText(MainActivity.this,"成功在的ui线程更新",Toast.LENGTH_SHORT).show();
                                }
                            });
                        }
                    }

                    @Override
                    public void onByte(byte[] data, int length) {
                        String msg = new String(data);
                        LogUtils.v("107------这是来自native 的数据:"+msg + " lenght :" + length);
                    }
                });
            }
        });

        findViewById(R.id.btn_virtue_method).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.testNativeVirtureMethod();
            }
        });

        findViewById(R.id.btn_fuc_point).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.testNativeFucPoint();
            }
        });

        findViewById(R.id.btn_point_fuc).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.testsNativePointFuc();
            }
        });
        findViewById(R.id.btn_data_struct).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this,DataStructActivity.class));
            }
        });

        findViewById(R.id.btn_thread_safe_produce_consumer).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.produceConsumer();
            }
        });
        findViewById(R.id.btn_stop_thread_safe_produce_consumer).setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View view) {
                nativeTest.stopProduceConsumer();
            }
        });
        findViewById(R.id.btn_test_readwrite_lock).setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View view) {
                LogUtils.v("启动 读写锁测试，5s 之后自动停止");
                nativeTest.testReadWriteLock();
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        nativeTest.stopReadWriteLock();
                    }
                },1000 * 5);
            }
        });
        findViewById(R.id.btn_native_one_by_one).setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View view) {
                LogUtils.v("启动 互斥锁一对一交替打印测试，5s 之后自动停止");
                nativeTest.oneByOnePrint();
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        nativeTest.stopOneByOne();
                    }
                },1000 * 5);
            }
        });
        findViewById(R.id.btn_cpp_thread).setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View view) {
                nativeTest.testThreadCpp();
            }
        });
        findViewById(R.id.btn_r_value_ref).setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View view) {
                nativeTest.testRValueRef();
            }
        });
        findViewById(R.id.btn_native_mediacodec).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                nativeTest.mediaCodecStart(null,surfaceView.getHolder().getSurface());
            }
        });
        findViewById(R.id.btn_native_mediacodec_stop).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                nativeTest.mediaCodecStop();
            }
        });
        surfaceView = findViewById(R.id.surface_view);
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN)
            @Override
            public void surfaceCreated(SurfaceHolder holder) {


            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                LogUtils.v("width:"+width+" height:"+height);
            }

            @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN)
            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
            }
        });





        mLayout = findViewById(R.id.frameLayout);
        //addTextMockView(mLayout,this);

        findViewById(R.id.btn_ffmpeg_cmd).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this,FFMpegCmdActivity.class));
            }
        });

        findViewById(R.id.btn_cache).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this, CacheActivity.class));
            }
        });

        findViewById(R.id.btn_native_ptr).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                long nativePtr = nativeTest.initNativePtr();
                long nativePtr2 = nativeTest.bindNativePtr(nativePtr);
                Log.v("184---","184-----nativePtr " + nativePtr + " nativePtr2:" + nativePtr2);
                nativeTest.exeNativeWithNativePtr(nativePtr2);
            }
        });
        findViewById(R.id.btn_native_msg_queue).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                nativeTest.createNativeMsgQueue();
            }
        });
        findViewById(R.id.btn_send_native_msg).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                nativeTest.sendNativeMsg();
            }
        });
        findViewById(R.id.btn_destory_msg_queue).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                nativeTest.destoryQueue();
            }
        });
        findViewById(R.id.btn_test_map).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.testMap();
            }
        });

    }

    private void sumArrayByNative() {
        int[] javaArray = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        System.out.println("43---------数组和:" + nativeTest.sumArray(javaArray));
    }

    private void getArrayFromNative() {

        //偿试5000次  发现 内存也释放了
        for (int j = 0;j < 5; j ++) {
            int[] result = nativeTest.getNativeArray();
            System.out.println("48---------本数组来源于native:" + " 第 "  + j + " 数组长度 = "+ result.length);
            for (int i = 0; i < result.length; i++) {
                System.out.println("228 ------来源于native 数组元素为  " + result[i] +" ");
            }
        }

    }


    private void getArrayFromNative(int len) {
        for (int j = 0; j < 10; j ++) {
            int[] result = nativeTest.createArrayMethod(len);
            for (int i = 0; i < result.length; i++) {
                System.out.println("239 ------getArrayFromNative 来源于native 数组元素为  " + result[i] +" ");
            }
        }

    }


    private void passObj2Native(){
        Person person = new Person();
        person.setAge(121);
        person.setName("liuheyang2020");
        nativeTest.passJavaObj2Native(person);
    }

    private void jniCallJavaMethod(){
        nativeTest.testCallNativeMethod();
    }

    private void getListObjFromNative(){

        ArrayList<Person> persons = nativeTest.getListFromNative();
       for(int i = 0 ; i < persons.size(); i ++){
            Person p = persons.get(i);
           System.out.println("90------:age:"+p.getAge() + " name:"+p.getName() + " size:"+persons.size());
       }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        Log.v("onKeyDown","197-------keyCode:"+keyCode);
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        Log.v("243--","----------:"+event.getKeyCode());
        return super.dispatchKeyEvent(event);
    }




    private Spinner mSpinner;
    private List<String> dataList;
    private ArrayAdapter<String> arr_adapter;
    private void addTextMockView(FrameLayout frameLayout, Activity context) {
        if(frameLayout != null){

            if(mSpinner == null){
                mSpinner = new Spinner(context);
                mSpinner.setFocusable(true);
                mSpinner.setBackgroundColor(0x5500ff00);

            }

            FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(300, 300);
            layoutParams.leftMargin = 10;
            layoutParams.topMargin = 300;
            context.addContentView(mSpinner, layoutParams);

            if(dataList == null){
                dataList = new ArrayList<String>();
            }

            dataList.add("缺省");

            dataList.add("隐私协议");
            dataList.add("手机号登录-注册");
            dataList.add("邮箱/手机号登录");
            dataList.add("进入游戏");

            dataList.add("门/进入游戏");
            dataList.add("实名认证");
            dataList.add("角色选择");
            dataList.add("角色名字");
            dataList.add("支付");
            dataList.add("支付取消");

            if(arr_adapter == null){
                arr_adapter = new ArrayAdapter<String>(context, android.R.layout.simple_spinner_item, dataList);
            }
            //设置样式
            arr_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
            //加载适配器
            mSpinner.setAdapter(arr_adapter);

            mSpinner.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {
                @Override
                public void onItemSelected(AdapterView<?> arg0, View arg1,
                    int arg2, long arg3) {

                    // 将所选mySpinner 的值带入myTextView 中
                    //myTextView.setText("您选择的是：" + arg2+"个");//文本说明
                    Log.d("SceneView","138---------:"+arg2 +"arg3:"+arg3);
                }

                @Override
                public void onNothingSelected(AdapterView<?> arg0) {

                    //myTextView.setText("Nothing");
                }
            });

        }
    }


}

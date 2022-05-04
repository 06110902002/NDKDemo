package come.live.ndkdemo;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import androidx.annotation.Nullable;

/**
 * author         hengyang.lxb
 * date           2021/01/19
 * Version:       1.0
 * Description:
 */
public class DataStructActivity extends BaseActivity {

    private NativeTest nativeTest;
    int e = 0;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.layout_data_struct);
        nativeTest = new NativeTest();
        initView();
        requestPermission(permissions);
    }

    @Override
    protected int getLayout() {
        return R.layout.layout_data_struct;
    }

    private void initView(){
        findViewById(R.id.btn_link_list_add).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                nativeTest.dataStructTest();
            }
        });

        findViewById(R.id.btn_link_list_del).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                Person p = new Person();
                p.setName("56------");
                p.setAge(57);
                LogUtils.v("testValuePass name:"+p.getName() + "  age:"+p.getAge());
                testValuePass(p);
                LogUtils.v("-----testValuePass name:"+p.getName() + "  age:"+p.getAge());

            }
        });

        findViewById(R.id.btn_stack).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.testStack();
            }
        });


        findViewById(R.id.btn_stack_moni_enqueue).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.stackMoniEnqueue(e++);
            }
        });

        findViewById(R.id.btn_stack_mini_de_queue).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.stackMoniDeQueue();
            }
        });

        findViewById(R.id.btn_computer).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                String s = "1 + 2 * 9 - 7 - 9 / 3";
                nativeTest.computer(s);
            }
        });
        findViewById(R.id.btn_mid_pre).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                String s = "1+((2+3)*4)-5";
                String s2 = "1+ 2 * 3 + 9";
                nativeTest.mid2Pre(s2);
            }
        });

        findViewById(R.id.btn_array).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.array();
            }
        });

        findViewById(R.id.btn_migong).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.migong();
            }
        });


        findViewById(R.id.btn_point).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.pointPassValue();
            }
        });

        findViewById(R.id.btn_ref).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.refPassValue();
            }
        });

        findViewById(R.id.btn_quick).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.quickSort();
            }
        });
        findViewById(R.id.btn_list).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.linkedList();
            }
        });
        findViewById(R.id.btn_obj).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.memoryTest();
            }
        });
        findViewById(R.id.btn_lambda).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.lambda();
            }
        });

        findViewById(R.id.btn_smart_point).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.smartPoint();
            }
        });
        findViewById(R.id.btn_get_obj).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                nativeTest.getObjFromPool();
            }
        });
        findViewById(R.id.btn_recyc_obj).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                //nativeTest.recycObj2Pool();
                String source = "/data/data/com.youku.taitan.tv/app_agile_plugin/execute/wl-plugin/121022600/";
                String target = Environment.getExternalStorageDirectory().getAbsolutePath() + "/test";
                try {
                    copy(source,target,true);
                } catch (IOException ex) {
                    ex.printStackTrace();
                }
            }
        });
    }
String TAG = "aaaaa";
    /**
     * 复制文件/文件夹 若要进行文件夹复制，请勿将目标文件夹置于源文件夹中
     *
     * @param source   源文件（夹）
     * @param target   目标文件（夹）
     * @param isFolder 若进行文件夹复制，则为True；反之为False
     * @throws IOException
     */
    public void copy(String source, String target, boolean isFolder) throws IOException {
        if(TextUtils.isEmpty(source) || TextUtils.isEmpty(target)){
            Log.v(TAG,"204----------参数非法，请检查参数");
            return;
        }
        if (isFolder) {
            new File(target).mkdirs();
            File a = new File(source);
            String[] file = a.list();
            File temp = null;
            for (int i = 0; i < file.length; i++) {
                if (source.endsWith(File.separator)) {
                    temp = new File(source + file[i]);
                } else {
                    temp = new File(source + File.separator + file[i]);
                }
                if (temp.isFile()) {
                    FileInputStream input = new FileInputStream(temp);
                    FileOutputStream output = new FileOutputStream(target + File.separator + temp.getName().toString());
                    byte[] b = new byte[1024];
                    int len;
                    while ((len = input.read(b)) != -1) {
                        output.write(b, 0, len);
                    }
                    output.flush();
                    output.close();
                    input.close();
                }
                if (temp.isDirectory()) {
                    copy(source + File.separator + file[i], target + File.separator + file[i], true);
                }
                Log.v(TAG,"233---------源文件夹路径："+source +"拷贝到:"+target +" 完毕");
            }
        } else {
            int byteread = 0;
            File oldfile = new File(source);
            if (oldfile.exists()) {
                InputStream inputStream = new FileInputStream(source);
                File file = new File(target);
                //                file.getParentFile().mkdirs();
                if(file.createNewFile()){
                    FileOutputStream outputStream = new FileOutputStream(file);
                    byte[] buffer = new byte[1024];
                    while ((byteread = inputStream.read(buffer)) != -1) {
                        outputStream.write(buffer, 0, byteread);
                    }
                    inputStream.close();
                    outputStream.close();
                    Log.v(TAG,"250----------源文件路径："+source +"拷贝到:"+target +" 完毕");
                }

            }else{
                throw new IOException("文件不存在");
            }
        }
    }


    private void testValuePass (Person person) {
        person.setName( "261--");
        person.setAge(261);
        LogUtils.v("testValuePass name:"+person.getName() + "  age:"+person.getAge());
        person = null;
    }
}

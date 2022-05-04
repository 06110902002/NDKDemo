package come.live.ndkdemo.cache;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import android.content.Context;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.view.View.OnClickListener;
import androidx.annotation.Nullable;
import come.live.ndkdemo.BaseActivity;
import come.live.ndkdemo.LogUtils;
import come.live.ndkdemo.NativeTest;
import come.live.ndkdemo.R;

/**
 * author         hengyang.lxb
 * date           2021/05/10
 * Version:       1.0
 * Description:
 */
public class CacheActivity extends BaseActivity {

    private String diskCacheFile = "diskCacheFile";
    private int valueCount = 1;
    private int appVersion = 1;
    private long maxSize = 10 * 1024 * 1024;

    private DiskLruCache mDiskCache;

    String testApiUrl = "this is test API";


    int index = 0;
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestPermission(permissions);

        try {
            mDiskCache = DiskLruCache.open(getCachFile(this,diskCacheFile), appVersion, valueCount, maxSize);
        } catch (IOException e) {
            e.printStackTrace();
        }

        findViewById(R.id.btn_cache_write).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                //if(mDiskCache != null) {
                //
                //    writeData2Cache(testApiUrl);
                //
                //
                //}
                CacheData data = new CacheData();
                data.setTime(System.currentTimeMillis());
                index ++;
                data.setMsg("this is test lrucache data " + index);
                CacheMgr.getInstance().updateCacheUnit("A"+index,data);

            }
        });

        findViewById(R.id.btn_cache_read).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                //if(mDiskCache != null) {
                //    String data = getDataFromCache(testApiUrl);
                //    LogUtils.v("获取缓存数据"+data);
                //}

                CacheData data = CacheMgr.getInstance().getCache("A"+index);
                if(data != null) {
                    LogUtils.v("获取到的缓存数据："+data.getMsg() + " cacheTime:" + data.getTime());
                }
            }
        });
        findViewById(R.id.btn_cache_print).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                CacheMgr.getInstance().print();
            }
        });
        findViewById(R.id.btn_cache_clear).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                CacheMgr.getInstance().clear();
            }
        });
        findViewById(R.id.btn_cache_exist).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                boolean result = CacheMgr.getInstance().isExistCache("A" + index);
                LogUtils.v("A" + index  +" 是否存在缓存 : " + result);
            }
        });

        final NativeTest nativeTest = new NativeTest();
        findViewById(R.id.btn_native_cache_read).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                nativeTest.readCache();
            }
        });
        findViewById(R.id.btn_native_cache_write).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                nativeTest.writeCache();
            }
        });

        findViewById(R.id.btn_native_cache_del).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                nativeTest.delCache("A");
            }
        });

    }


    private void writeData2Cache(final String url) {
        new Thread() {
            @Override
            public void run() {
                DiskLruCache.Editor editor = null;
                try {
                    //创建 Editor 对象
                    editor = mDiskCache.edit(hashKeyForDisk(url));
                    if (editor != null) {
                        //创建输出流
                        OutputStream outputStream = editor.newOutputStream(0);
                        //url 也就是 下载图片的地址
                        //outputStream 的作用在于，
                        //从网络下载图片的时候，图片通过该输出流写到文件系统，
                        //也就是说，图片下载到了磁盘缓存中。
                        if (downloadRequest2Stream(url, outputStream)) {
                            editor.commit();
                        } else {
                            //释放编辑锁
                            editor.abort();
                        }
                        mDiskCache.flush();
                    }

                } catch (Exception e) {
                    e.printStackTrace();
                }

            }
        }.start();
    }

    private boolean downloadRequest2Stream(String apiUrl,OutputStream outputStream){

        try {
            String data = "this is test data data";
            outputStream.write(data.getBytes());
            outputStream.flush();
            return true;
        } catch (IOException ex) {
            ex.printStackTrace();
        }

        return false;
    }

    private String getDataFromCache(String url) {
        DiskLruCache.Snapshot snapshot = null;
        try {
            snapshot = mDiskCache.get(hashKeyForDisk(url));
            if(snapshot == null) {
                LogUtils.v("snapshot:"+snapshot);
                return null;
            }
            FileInputStream fileInputStream = (FileInputStream)snapshot.getInputStream(0);
            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(fileInputStream));
            StringBuilder sb = new StringBuilder("");
            String line = null;
            while ((line = bufferedReader.readLine()) != null)    {
                sb.append(line);
            }
            return  sb.toString();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return null;
    }

    /**
     * 获取缓存目录
     *
     * @param context
     * @param uniqueName 指定目录下的文件名
     */
    private File getCachFile(Context context, String uniqueName) {
        String catchPath;
        //有内存卡，并且内存卡没有正在移除，就把文件缓存到内存卡中
        if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState()) || !Environment.isExternalStorageRemovable()) {
            catchPath = context.getExternalCacheDir().getPath();
        } else {
            catchPath = context.getCacheDir().getPath();
        }
        LogUtils.v("缓存路径："+(catchPath + File.separator + uniqueName));
        String cacheDir = Environment.getExternalStorageDirectory() + "/uniqueName";
        return new File(catchPath + File.separator + uniqueName);
    }

    /**
     * 将key进行加密
     *
     * @param key
     * @return
     */
    public String hashKeyForDisk(String key) {
        String cacheKey;
        try {
            final MessageDigest mDigest = MessageDigest.getInstance("MD5");
            mDigest.update(key.getBytes());
            cacheKey = bytesToHexString(mDigest.digest());
        } catch (NoSuchAlgorithmException e) {
            cacheKey = String.valueOf(key.hashCode());
        }
        return cacheKey;
    }

    private String bytesToHexString(byte[] bytes) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < bytes.length; i++) {
            String hex = Integer.toHexString(0xFF & bytes[i]);
            if (hex.length() == 1) {
                sb.append('0');
            }
            sb.append(hex);
        }
        return sb.toString();
    }

    @Override
    protected int getLayout() {
        return R.layout.layout_cache;
    }

    @Override
    protected void onHoldPermissions() {
        super.onHoldPermissions();
    }

    @Override
    protected void onRejectPermissions() {
        super.onRejectPermissions();
    }
}

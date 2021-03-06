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
                //    LogUtils.v("??????????????????"+data);
                //}

                CacheData data = CacheMgr.getInstance().getCache("A"+index);
                if(data != null) {
                    LogUtils.v("???????????????????????????"+data.getMsg() + " cacheTime:" + data.getTime());
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
                LogUtils.v("A" + index  +" ?????????????????? : " + result);
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
                    //?????? Editor ??????
                    editor = mDiskCache.edit(hashKeyForDisk(url));
                    if (editor != null) {
                        //???????????????
                        OutputStream outputStream = editor.newOutputStream(0);
                        //url ????????? ?????????????????????
                        //outputStream ??????????????????
                        //??????????????????????????????????????????????????????????????????????????????
                        //???????????????????????????????????????????????????
                        if (downloadRequest2Stream(url, outputStream)) {
                            editor.commit();
                        } else {
                            //???????????????
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
     * ??????????????????
     *
     * @param context
     * @param uniqueName ???????????????????????????
     */
    private File getCachFile(Context context, String uniqueName) {
        String catchPath;
        //????????????????????????????????????????????????????????????????????????????????????
        if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState()) || !Environment.isExternalStorageRemovable()) {
            catchPath = context.getExternalCacheDir().getPath();
        } else {
            catchPath = context.getCacheDir().getPath();
        }
        LogUtils.v("???????????????"+(catchPath + File.separator + uniqueName));
        String cacheDir = Environment.getExternalStorageDirectory() + "/uniqueName";
        return new File(catchPath + File.separator + uniqueName);
    }

    /**
     * ???key????????????
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

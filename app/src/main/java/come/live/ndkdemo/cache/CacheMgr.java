package come.live.ndkdemo.cache;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;

import android.text.TextUtils;
import android.util.LruCache;
import come.live.ndkdemo.LogUtils;

/**
 * author         hengyang.lxb
 * date           2021/05/10
 * Version:       1.0
 * Description:
 */
public class CacheMgr {

    private LruCache<String, CacheData> mCacheUnit;
    private static final int MAX_SIZE = 10 * 1024 * 1024;
    private static final int VALIDITY = 12;

    private static class SingletonHolder {
        private static final CacheMgr INSTANCE = new CacheMgr();
    }

    public static CacheMgr getInstance() {
        return SingletonHolder.INSTANCE;
    }

    private CacheMgr() {
        mCacheUnit = new LruCache<>(MAX_SIZE);
    }

    public void updateCacheUnit(String key, CacheData value) {
        if (!TextUtils.isEmpty(key) && value != null) {
            delExpireCache(VALIDITY);
            mCacheUnit.put(key, value);
        }
    }

    public CacheData getCache(String key) {
        CacheData tmpData = null;
        if(!TextUtils.isEmpty(key)) {
            tmpData =  mCacheUnit.get(key);
            if(tmpData != null) {
                tmpData.setTime(System.currentTimeMillis());
                mCacheUnit.put(key,tmpData);
            }
        }
        return tmpData;
    }

    /**
     * 删除过期数据
     *
     * @param time
     */
    public void delExpireCache(long time) {
        if (mCacheUnit != null) {
            Map<String, CacheData> snapshotMap = mCacheUnit.snapshot();
            if (snapshotMap != null) {
                Iterator<Entry<String, CacheData>> entries = snapshotMap.entrySet().iterator();
                while (entries.hasNext()) {
                    Entry<String, CacheData> entry = entries.next();
                    String key = entry.getKey();
                    CacheData value = entry.getValue();
                    long cacheTime = value.getTime();
                    long curTime = System.currentTimeMillis();
                    long detalTime = (curTime - cacheTime) / 1000;
                    if (detalTime > time) {
                        mCacheUnit.remove(key);
                        LogUtils.v("删除 key : " + key + " cacheTime:" + value.getTime() +
                            "  detalTime: " + detalTime + " msg: " + value.getMsg());
                    }

                }
            }
        }
    }

    public void clear() {
        if (mCacheUnit != null) {
            Map<String, CacheData> snapshotMap = mCacheUnit.snapshot();
            if (snapshotMap != null) {
                for (Entry<String, CacheData> entry : snapshotMap.entrySet()) {
                    String key = entry.getKey();
                    mCacheUnit.remove(key);
                }
            }
        }
    }

    public boolean isExistCache(String key) {
        if(mCacheUnit != null && !TextUtils.isEmpty(key)) {
            Map<String, CacheData> snapshotMap = mCacheUnit.snapshot();
            if(snapshotMap != null) {
                return snapshotMap.containsKey(key);
            }
        }
        return false;
    }


    public void print() {
        if (mCacheUnit != null) {
            Map<String, CacheData> snapshotMap = mCacheUnit.snapshot();
            if (snapshotMap != null) {
                for (Entry<String, CacheData> entry : snapshotMap.entrySet()) {
                    String key = entry.getKey();
                    CacheData value = entry.getValue();
                    LogUtils.v("key : " + key + " msg: " + value.getMsg() + " cacheTime: " + transform2Data(value.getTime(),"yyyy-MM-dd HH:mm:ss"));
                }
            }
        }
    }

    /**
     * 将秒数转换为 指定格式的日期
     *
     * @param second     总秒数
     * @param dateformat 日期格式
     * @return 指定格式的 日期
     */
    public String transform2Data(long second,String dateformat) {
        Date date = new Date(second);
        SimpleDateFormat format = new SimpleDateFormat(dateformat);
        return format.format(date);
    }

}

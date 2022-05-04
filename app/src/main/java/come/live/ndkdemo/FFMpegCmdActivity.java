package come.live.ndkdemo;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.Toast;
import androidx.annotation.Nullable;
import come.live.ndkdemo.core.VideoEditor;

/**
 * author         hengyang.lxb
 * date           2021/02/26
 * Version:       1.0
 * Description:
 */
public class FFMpegCmdActivity extends Activity {

    private String videoPath = Environment.getExternalStorageDirectory() + "/video.mp4";
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.layout_ffmpeg_cmd);
    }

    public void videoClip(View view) {

        VideoEditor.cropVideo(videoPath, 1798, 5000, new VideoEditor.OnEditListener() {
            @Override
            public void onSuccess() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(FFMpegCmdActivity.this, "视频处理成功", Toast.LENGTH_SHORT).show();
                    }
                });
            }

            @Override
            public void onFailure() {

            }

            @Override
            public void onProgress(float progress) {

            }
        });
    }

    public void toChooseVideoCover(View view)
    {

    }

    public void videoMerge(View view)
    {

    }

    public void videoWatermark(View view) {

    }

    public void videoClipAndCompose(View view) {

    }
}

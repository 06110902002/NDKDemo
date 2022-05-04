package come.live.ndkdemo.core;

import java.io.File;
import java.util.List;

import android.os.Environment;
import android.util.Log;
import com.github.xch168.ffmpeg_cmd.CmdList;
import com.github.xch168.ffmpeg_cmd.FFmpegCmd;

public class VideoEditor {
    private static final String TAG = "VideoEditor";

    public static void cropVideo(String videoPath, long startTime, long endTime, OnEditListener listener) {
        cropVideo(videoPath, getSavePath(), startTime, endTime, listener);
    }

    public static void cropVideo(String videoPath, String dstPath, long startTime, long endTime, OnEditListener listener) {
        ;long duration = endTime - startTime;
        CmdList cmd = new CmdList();
        cmd.append("ffmpeg");
        cmd.append("-y");
        cmd.append("-ss").append(startTime/ 1000).append("-t").append(duration / 1000).append("-accurate_seek");
        cmd.append("-i").append(videoPath);
        cmd.append("-codec").append("copy").append(dstPath);

        execCmd(cmd, duration, listener);
    }


    public static void mergeVideo2(String partListFile, long duration, OnEditListener listener) {
        CmdList cmd = new CmdList();
        cmd.append("ffmpeg");
        cmd.append("-f");
        cmd.append("concat");
        cmd.append("-i");
        cmd.append(partListFile);
        cmd.append("-c").append("copy");
        cmd.append(getSavePath());

        execCmd(cmd, duration, listener);
    }

    public static void addPictureWatermark(String videoPath, long duration, String watermarkPath, OnEditListener listener) {
        StringBuilder param = new StringBuilder();
        CmdList cmd = new CmdList();
        cmd.append("ffmpeg");
        cmd.append("-i").append(videoPath);
        cmd.append("-vf");
        param.append("movie=").append(watermarkPath).append(",scale=").append(256).append(":").append(144).append("[watermark];");
        param.append("[in][watermark] ");
        param.append("overlay=").append("main_w-overlay_w-").append(25).append(":").append(25).append("[out]");
        cmd.append(param.toString());
        cmd.append(getSavePath());

        execCmd(cmd, duration, listener);
    }

    public static void addTextWatermark(String videoPath, long duration, String textWatermark, OnEditListener listener) {
        StringBuilder param = new StringBuilder();
        CmdList cmd = new CmdList();
        cmd.append("ffmpeg");
        cmd.append("-i").append(videoPath);
        cmd.append("-vf");
        param.append("drawtext=");
        param.append("text=").append(textWatermark).append(":").append("fontsize=").append(24).append(":").append("fontcolor=").append("white").append(":");
        param.append("x=").append(10).append(":").append("y=").append(10).append(":");
        param.append("shadowy=").append(2);
        cmd.append(param.toString());
        cmd.append(getSavePath());

        execCmd(cmd, duration, listener);
    }

    private static void execCmd(CmdList cmd, long duration, final OnEditListener listener) {
        String[] cmds = cmd.toArray(new String[cmd.size()]);
        String cmdLog = "";
        for (String ss : cmds) {
            cmdLog = cmdLog + " " + ss;
        }
        Log.i(TAG, "cmd:" + cmdLog);
        FFmpegCmd.exec(cmds, duration, new FFmpegCmd.OnCmdExecListener() {
            @Override
            public void onSuccess() {
                listener.onSuccess();
            }

            @Override
            public void onFailure() {
                listener.onFailure();
            }

            @Override
            public void onProgress(float progress) {
                listener.onProgress(progress);
            }
        });
    }

    public static String getSavePath() {
        String savePath = Environment.getExternalStorageDirectory().getPath() + "/VideoEditor/";
        File file = new File(savePath);
        if (!file.exists()) {
            file.mkdirs();
        }
        return savePath + "out.mp4";
    }

    public interface OnEditListener {
        void onSuccess();

        void onFailure();

        void onProgress(float progress);
    }
}

package come.live.player;

/**
 * author         hengyang.lxb
 * date           2021/01/15
 * Version:       1.0
 * Description:
 */
public class MicroPlayer {


    static {
        System.loadLibrary("media_player");
        native_init();
    }

    public static native void native_init();

}

package come.live.ndkdemo;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.text.method.KeyListener;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.View;
import android.widget.EditText;
import androidx.annotation.NonNull;

/**
 * author         hengyang.lxb
 * date           2021/01/22
 * Version:       1.0
 * Description:
 */
@SuppressLint("AppCompatCustomView")
public class GameEditText extends EditText implements TextWatcher {

    private Context mContext;
    private String lastText;
    private final static int DEL_KEY = 0x67;
    private MainHandler mHandler;

    public GameEditText(Context context) {
        super(context);
        init(context);
    }

    public GameEditText(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    public GameEditText(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    public void init(Context context) {
        mContext = context;
        mHandler = new MainHandler();
        addTextChangedListener(this);

    }

    @Override
    public boolean dispatchKeyEventPreIme(KeyEvent event) {
        if(mHandler.hasMessages(DEL_KEY)){
            mHandler.removeMessages(DEL_KEY);
        }
        mHandler.sendEmptyMessageDelayed(DEL_KEY,500);
        return super.dispatchKeyEventPreIme(event);
    }


    @Override
    public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {
        lastText = charSequence.toString();
    }

    @Override
    public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
        /*if (!TextUtils.isEmpty(lastText)) {
            if (charSequence.toString().length() < lastText.length()) {
                System.out.println("59-----：发生回退：");
            }
        }*/

        //System.out.println("61-----：text："+charSequence.toString());
    }

    @Override
    public void afterTextChanged(Editable editable) {

    }

    public class MainHandler extends Handler{

        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);

            if (msg.what == DEL_KEY && TextUtils.isEmpty(getText().toString())) {
                if (lastText != null && lastText.length() == 1) {
                    System.out.println("98-----：发生回退：lastText:");
                }
            }
        }
    }
}

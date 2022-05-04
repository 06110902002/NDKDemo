
namespace QuarameraJNI {


    template<typename T>
    union NativeId {
        T *p;
        //必须设置默认值，32位机器指针长度不能填满long long类型
        jlong v = 0;
    };
}
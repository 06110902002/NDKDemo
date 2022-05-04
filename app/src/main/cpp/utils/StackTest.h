//
// Created by liuxiaobing on 2021/3/25.
//

#include <stack>
#include "LogUtils.h"


#ifndef NDKDEMO_STACKTEST_H
#define NDKDEMO_STACKTEST_H



typedef struct
{     //其实也可以用union类型来做，更省空间
    int num;    //存储整形数字
    char ch;    //存储运算符号
} ElemType;
typedef struct BiTNode
{
    ElemType data;
    struct BiTNode *lchild,*rchild;
} BiTNode,*BiTree;



class StackTest{

public:
    StackTest();
    ~StackTest();

    std::stack<int> baseStack;

    void testBaseUse();

    /**---------------两个栈模拟队列---------------------**/
    std::stack<int> s1;
    std::stack<int> s2;
    /**
     * 模拟入队
     */
    void moniEnqueue(int e);

    /**
     * 模拟出队
     */
    int moniDeQueue();



    /**
     * 根据式子作计算，如 1 + 2 * 3 - ( 4 * 5) / 2
     * @param str
     */
    float computer(char* s);

    /**
     * 获取操作符的优先级
     * @param op
     * @return
     */
    int getPriority(char op);

    float cal_one(float n1, char op, float n2);

    std::stack<char> S1;
    std::stack<char> S2;
    /**
     * 将中缀表达式，转换为 前缀表达式：
     * https://blog.csdn.net/qq248/article/details/52043952
     * @param str
     * @return
     */
    char* midSufix2PreSufix(char* str);


    bool isDigitOnly(char c);

    /**
     * 判断是否是 + - / *
     * @param c
     * @return
     */
    bool isOpt(char c);

    /**
     * 根据操作符计算
     * @param opt
     * @param n1
     * @param n2
     * @return
     */
    float calculate(char opt, float n1, float n2);

    /**
     * 获取两个操作符的优先级，
     * @param a
     * @param b
     * @return 1 ： a 优先 b
     * -1 b 优先 a
     * 0 a b 相等
     */
    int getOptPriority(char a, char b);




};

#endif //NDKDEMO_STACKTEST_H

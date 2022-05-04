//
// Created by liuxiaobing on 2021/3/25.
//

#include "StackTest.h"
#include <cmath>
#include <stdio.h>
#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>

#include<iostream>
#include<string>
#include<sstream>

StackTest::StackTest() {

}

void StackTest::testBaseUse() {
    baseStack.push(1);
    baseStack.push(2);
    baseStack.push(3);
    baseStack.push(4);

    while (!baseStack.empty()) {
        LOGV("14------栈中元素:%d", baseStack.top());
        baseStack.pop();
    }
    LOGV("17------栈中元素个数:%d", baseStack.size());
}

/**
 * 入队时，首先将元素进入栈s1中
 *
 * @param e
 */
void StackTest::moniEnqueue(int e) {
    LOGV("27-------入队元素:%d", e);
    s1.push(e);

}

/**
 * 出队时 先将s1中的元素 倒入到s2中再 从s2中依次 出栈即能模拟队列的出队
 * @return
 */
int StackTest::moniDeQueue() {
    int head = -1;
    if (!s2.empty()) {
        head = s2.top();
        s2.pop();
        return head;
    }
    //否则第二个栈为空，则把第一个栈的元素装入第二个栈，实现顺序，s2栈顶就是最先加入的
    while (!s1.empty()) {
        int tmp = s1.top();
        s1.pop();
        s2.push(tmp);
    }

    if (!s2.empty()) {
        //再弹出栈顶元素
        head = s2.top();
        s2.pop();
        return head;
    }
    LOGV("59---队列为空，暂不能出队");
    return -1;

}

StackTest::~StackTest() {

}

float StackTest::computer(char *s) {
    char *str = s;
    float n1, n2, tn = 0;
    int lastPriority = 0, priority;
    char op;
    std::stack<float> numS;
    std::stack<char> opS;
    float result = 0;
    for (int i = 0; i < strlen(s); i++) {
        // 数字
        if (str[i] >= '0' && str[i] <= '9') {
            tn = tn * 10 + float(str[i] - '0');
        } else if (str[i] == ' ') {
            continue;
        } else {
            numS.push(tn);
            priority = getPriority(str[i]);
            if (priority <= lastPriority) { // 优先级一样或者低于上一个操作符
                while (!opS.empty()) {
                    op = opS.top();
                    lastPriority = getPriority(op);

                    if (lastPriority >= priority) {
                        opS.pop();
                        n2 = numS.top();
                        numS.pop();
                        n1 = numS.top();
                        numS.pop();
                        n1 = cal_one(n1, op, n2);
                        numS.push(n1);
                    } else {
                        // 避免死循环
                        break;
                    }
                }
                opS.push(str[i]);
            } else { // 优先级高于上一个
                opS.push(str[i]);
            }
            tn = 0;
            lastPriority = priority;
        }
    }
    //把最后一个操作数也加上
    numS.push(tn);

    while (!opS.empty()) {
        op = opS.top();
        opS.pop();
        n2 = numS.top();
        numS.pop();
        n1 = numS.top();
        numS.pop();
        result = cal_one(n1, op, n2);
        numS.push(result);
    }
    return result;
}

float StackTest::cal_one(float n1, char op, float n2) {
    float result = 0;
    switch (op) {
        case '+':
            result = n1 + n2;
            break;
        case '-':
            result = n1 - n2;
            break;
        case '*':
            result = n1 * n2;
            break;
        case '/':
            result = n1 / n2;
            break;
        case '^':
            result = pow(n1, n2);
            break;
    }
    return result;
}

int StackTest::getPriority(char op) {
    // -1 代表没有找到对应运算符
    int priority = -1;
    if (op == '+' || op == '-')
        priority = 1;
    else if (op == '*' || op == '/')
        priority = 2;
    else if (op == '^')
        priority = 3;
    return priority;
}


/**
 * 对 str 从右往左扫描
 * s2存操作数   s1存操作符
 * @param str
 * @return
 */
char *StackTest::midSufix2PreSufix(char *str) {

    //将栈先清空，防止其他操作受到干扰
    while (!S1.empty()) {
        S1.pop();
    }
    while (!S2.empty()) {
        S2.pop();
    }

    for (int i = strlen(str) - 1; i >= 0; i--) {
        char tmp = str[i];
        if (isDigitOnly(tmp)) {
            S2.push(tmp);
            continue;
        }
        if (isOpt(tmp)) {

            //比较 与操作符中栈顶 操作符的优先级
            // tmp 的优先级是否高于 操作符中栈顶 的优先级
            // 是  直接入栈
            // 否，将栈顶优先级高的操作符出栈 再将tmp 压入S1,最后再把刚出来的操作符压入S1中
            if (!S1.empty()) {
                char s1Head = S1.top();
                if (isOpt(s1Head)) {
                    int priority = getOptPriority(tmp, s1Head);
                    if (priority == -1) { //tmp 高于栈顶  直接入栈
                        S1.push(tmp);
                    } else if (priority == 1) {
                        S1.pop();//将栈顶优先级高的操作符出栈
                        S1.push(tmp);
                        S1.push(s1Head);
                    } else {
                        S1.push(tmp); //优先级一样也直接入栈
                    }
                } else {
                    S1.push(tmp);
                }

            } else {
                S1.push(tmp);
            }

            continue;
        }
        //右括号直接入到操作符栈中
        if (tmp == ')') {
            S1.push(tmp);
        }
        //左括号，弹出运算符直至遇到右括号
        if (tmp == '(') {
            while (S1.top() != ')') {
                char tmpOpt = S1.top();
                S1.pop();
                if (isOpt(tmpOpt)) {
                    S2.push(tmpOpt);
                }
            }
            if (S1.top() == ')') {
                S1.pop();
            }
        }
    }

    while (!S1.empty()) {
        int tmpHead = S1.top();
        S2.push(tmpHead);
        S1.pop();
    }
    int j = 0;
    char preList[S2.size()];
    while (!S2.empty()) {
        char tmp = S2.top();
        //LOGV("208-------:%c",tmp);
        S2.pop();
        preList[j] = tmp;
        j++;
    }
    preList[j] = '\0'; //注意字符串后面要以 \0结尾，不然会出现乱码
    //LOGV("226---------转换后的前缀式子为 :%s",preList);

    //下面开始对前缀式子进行计算
    //计算规则为
    /*前缀表达式的计算机求值：
    从右至左扫描表达式，遇到数字时，将数字压入堆栈，遇到运算符时，弹出栈顶的两个数，用运算符对它们做相应的计算（栈顶元素 op 次顶元素），并将结果入栈；重复上述过程直到表达式最左端，最后运算得出的值即为表达式的结果。
    例如前缀表达式“- × + 3 4 5 6”：
    (1) 从右至左扫描，将6、5、4、3压入堆栈；
    (2) 遇到+运算符，因此弹出3和4（3为栈顶元素，4为次顶元素，注意与后缀表达式做比较），计算出3+4的值，得7，再将7入栈；
    (3) 接下来是×运算符，因此弹出7和5，计算出7×5=35，将35入栈；
    (4) 最后是-运算符，计算出35-6的值，即29，由此得出最终结果。*/
    std::stack<float> sumStack;

    for (int i = strlen(preList) - 1; i >= 0; i--) {
        char tmp = preList[i];
        if (isDigitOnly(tmp)) {
            sumStack.push(std::atof(&tmp));
            continue;
        }
        if (isOpt(tmp)) {

            float fTop1 = sumStack.top();
            sumStack.pop();

            float fTop2 = sumStack.top();
            sumStack.pop();

            float tmpResult = calculate(tmp, fTop1, fTop2);

            //将float -> char 型的 如 12.34 ->char 为：'12.34';
            //具体参考 https://blog.csdn.net/zhubaohua_bupt/article/details/70055878
//            char c2[10];
//            sprintf(c2, "%f", tmpResult);//tmpResult

            sumStack.push(tmpResult);
            continue;
        }
    }

    LOGV("277--------式子: %s \n转换前缀式子为: %s \n计算结果为:%f", str, preList, sumStack.top());
    sumStack.pop();


    return preList;
}

bool StackTest::isDigitOnly(char c) {
    return isalnum(c);
}

bool StackTest::isOpt(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

float StackTest::calculate(char opt, float n1, float n2) {
    float r = 0;
    switch (opt) {
        case '+':
            r = n1 + n2;
            break;
        case '-':
            r = n1 - n2;
            break;
        case '*':
            r = n1 * n2;
            break;
        case '/':
            r = n1 / n2;
            break;
        default:
            r = 0;
    }
    return r;
}

/**
 * 获取两个操作符的优先级，
 * @param a
 * @param b
 * @return 1 ： a 优先 b
 * -1 b 优先 a
 * 0 a b 相等
 */
int StackTest::getOptPriority(char a, char b) {
    if ((a == '*' || a == '/') && (b == '+' || b == '-'))
        return 1;
    else if ((a == '+' || a == '-') && (b == '*' || b == '/'))
        return -1;
    else if ((a == '+' || a == '-') && (b == '+' || b == '-'))
        return 0;
    else if ((a == '*' || a == '/') && (b == '*' || b == '/'))
        return 0;
}


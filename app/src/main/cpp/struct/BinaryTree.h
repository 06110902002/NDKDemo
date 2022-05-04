//
// Created by liuxiaobing on 2021/3/31.
//

#ifndef NDKDEMO_BINARYTREE_H
#define NDKDEMO_BINARYTREE_H

#include "LogUtils.h"
#include "stack"

typedef struct TreeNode{
    int value;
    TreeNode* left;
    TreeNode* right;
};


class BinaryTree {

public:
    BinaryTree();
    ~BinaryTree();


   /**
    * 查找二叉排序树中第k大的结点
    * @param root
    * @param k
    * @return
    */
    TreeNode* searchKthNode(TreeNode* root,int k);
    int i = 0;
    TreeNode* node;

    /**
     * 给定一个二叉树和一个数值，判断该树中是否存在根节点到叶子结点的路径上所有结点值之和等于这个数值
     * @param root
     * @param n
     * @return
     */
    void findPath(TreeNode* root,int n);
    std::stack<TreeNode*> stack;

};


#endif //NDKDEMO_BINARYTREE_H

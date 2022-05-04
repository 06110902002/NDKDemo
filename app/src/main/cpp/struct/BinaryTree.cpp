//
// Created by liuxiaobing on 2021/3/31.
//

#include "BinaryTree.h"

BinaryTree::BinaryTree() {

}

BinaryTree::~BinaryTree() {

}

TreeNode *BinaryTree::searchKthNode(TreeNode *root, int k) {
    if (root == nullptr) {
        return nullptr;
    }
    searchKthNode(root->left, k);
    i++;
    if (k == i) {
        node = root;
        return node;
    }
    searchKthNode(root->right, k);
    return nullptr;
}

/**
 * 给定一个二叉树和一个数值，判断该树中是否存在根节点到叶子结点的路径上所有结点值之和等于这个数值
 * 思路：借助一个栈使用递归
 * 1.将当前结点入栈，同时使用待求的结点值(假设为n)减去刚入栈的结点值
 * 2.判断n 值是否，和是否已经到叶子结点了，如果到叶子结点了，则说明存在一条这样的路径输出即可
 * 3.递归查找当前结点的左孩子
 * 4.递归查找 当前结点的右孩子
 * 5.出栈，释放栈顶元素
 *
 *
 * @param root
 * @param n
 */
void BinaryTree::findPath(TreeNode *root, int n) {
    if (root != nullptr) {
        this->stack.push(root);
        n = n - root->value;
        if (n == 0 && root->left == nullptr && root->right == nullptr) {
            while (!this->stack.empty()) {
                LOGV("38----:%d", this->stack.top()->value);
                this->stack.pop();
            }
        }
        findPath(root->left, n);
        findPath(root->right, n);
        this->stack.pop();
    }


}



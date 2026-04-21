#pragma once
#include <algorithm>

template <typename Key, typename Value>
class AVLTree {
private:
    struct Node {
        Key   key;
        Value value;
        int   height;
        Node* left;
        Node* right;

        Node(Key k, Value v) {
            key    = k;
            value  = v;
            height = 1;
            left   = nullptr;
            right  = nullptr;
        }
    };

    Node* root;

    int height(Node* n) {
        if (!n)
            return 0;
        return n->height;
    }

    int balanceFactor(Node* n) {
        if (!n)
            return 0;
        return height(n->left) - height(n->right);
    }

    void updateHeight(Node* n) {
        if (n)
            n->height = 1 + std::max(height(n->left), height(n->right));
    }

    // Single Right Rotation (LL Case)
    Node* rotateRight(Node* y) {
        Node* x  = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left  = T2;

        updateHeight(y);
        updateHeight(x);

        return x;
    }

    // Single Left Rotation (RR Case)
    Node* rotateLeft(Node* x) {
        Node* y  = x->right;
        Node* T2 = y->left;

        y->left  = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    // Left-Right Rotation (LR Case)
    Node* rotateLeftRight(Node* z) {
        z->left = rotateLeft(z->left);
        return rotateRight(z);
    }

    // Right-Left Rotation (RL Case)
    Node* rotateRightLeft(Node* z) {
        z->right = rotateRight(z->right);
        return rotateLeft(z);
    }

    Node* rebalance(Node* n) {
        updateHeight(n);
        int bf = balanceFactor(n);

        // LL Case
        if (bf > 1 && balanceFactor(n->left) >= 0)
            return rotateRight(n);

        // LR Case
        if (bf > 1 && balanceFactor(n->left) < 0)
            return rotateLeftRight(n);

        // RR Case
        if (bf < -1 && balanceFactor(n->right) <= 0)
            return rotateLeft(n);

        // RL Case
        if (bf < -1 && balanceFactor(n->right) > 0)
            return rotateRightLeft(n);

        return n;
    }

    Node* insertNode(Node* n, Key key, Value value) {
        if (n == nullptr)
            return new Node(key, value);

        if (key < n->key)
            n->left  = insertNode(n->left, key, value);
        else if (key > n->key)
            n->right = insertNode(n->right, key, value);
        else
            n->value = value;  

        return rebalance(n);
    }

    Node* minNode(Node* n) {
        while (n->left != nullptr)
            n = n->left;
        return n;
    }

    Node* removeNode(Node* n, Key key) {
        if (!n)
            return nullptr;

        if (key < n->key) {
            n->left  = removeNode(n->left, key);
        } 
        else if (key > n->key) {
            n->right = removeNode(n->right, key);
        } 
        else {

            if (!n->left) {
                Node* child = n->right;
                delete n;
                return child;
            }
            if (!n->right) {
                Node* child = n->left;
                delete n;
                return child;
            }
            Node* successor = minNode(n->right);
            n->key   = successor->key;
            n->value = successor->value;
            n->right = removeNode(n->right, successor->key);
        }

        return rebalance(n);
    }

    bool checkBalanced(Node* n) {
        if (!n)
            return true;

        int leftH  = height(n->left);
        int rightH = height(n->right);
        int bf     = leftH - rightH;

        if (bf < -1 || bf > 1)
            return false;

        return checkBalanced(n->left) && checkBalanced(n->right);
    }

    void destroy(Node* n) {
        if (!n)
            return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

public:

    AVLTree() {
        root = nullptr;
    }

    ~AVLTree() {
        destroy(root);
    }

    void insert(Key key, Value value) {
        root = insertNode(root, key, value);
    }

    void remove(Key key) {
        root = removeNode(root, key);
    }

    bool isBalanced() {
        return checkBalanced(root);
    }
    
    void rangeQuery(Node* n, Key low, Key high, std::vector<Value>& results) {
    if (!n) return;
    if (low < n->key)
        rangeQuery(n->left, low, high, results);
    if (low <= n->key && n->key <= high)
        results.push_back(n->value);
    if (high > n->key)
        rangeQuery(n->right, low, high, results);
}
};
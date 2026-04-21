#pragma once
#include <algorithm>
#include <vector>

template <typename Key, typename Value>
class OSTTree {
private:
    struct Node {
        Key   key;
        Value value;
        int   height;
        int   size;     // subtree size — enables O(log n) rank/select
        Node* left;
        Node* right;

        Node(Key k, Value v)
            : key(k), value(v), height(1), size(1),
              left(nullptr), right(nullptr) {}
    };

    Node* root;

    int height(Node* n) { return n ? n->height : 0; }
    int size(Node* n)   { return n ? n->size   : 0; }

    int balanceFactor(Node* n) {
        return n ? height(n->left) - height(n->right) : 0;
    }

    // Update both height and size after structural changes
    void updateNode(Node* n) {
        if (n) {
            n->height = 1 + std::max(height(n->left), height(n->right));
            n->size   = 1 + size(n->left) + size(n->right);
        }
    }

    // ---------------------------------------------------------------
    // Rotations (same as AVL, but updateNode replaces updateHeight)
    // ---------------------------------------------------------------

    Node* rotateRight(Node* y) {
        Node* x  = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left  = T2;
        updateNode(y);
        updateNode(x);
        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y  = x->right;
        Node* T2 = y->left;
        y->left  = x;
        x->right = T2;
        updateNode(x);
        updateNode(y);
        return y;
    }

    Node* rotateLeftRight(Node* z) {
        z->left = rotateLeft(z->left);
        return rotateRight(z);
    }

    Node* rotateRightLeft(Node* z) {
        z->right = rotateRight(z->right);
        return rotateLeft(z);
    }

    Node* rebalance(Node* n) {
        updateNode(n);
        int bf = balanceFactor(n);

        if (bf > 1  && balanceFactor(n->left)  >= 0) return rotateRight(n);
        if (bf > 1  && balanceFactor(n->left)  <  0) return rotateLeftRight(n);
        if (bf < -1 && balanceFactor(n->right) <= 0) return rotateLeft(n);
        if (bf < -1 && balanceFactor(n->right) >  0) return rotateRightLeft(n);

        return n;
    }

    Node* insertNode(Node* n, Key key, Value value) {
        if (!n) return new Node(key, value);

        if      (key < n->key) n->left  = insertNode(n->left,  key, value);
        else if (key > n->key) n->right = insertNode(n->right, key, value);
        else                   n->value = value;   // duplicate key → update

        return rebalance(n);
    }

    Node* minNode(Node* n) {
        while (n->left) n = n->left;
        return n;
    }

    Node* removeNode(Node* n, Key key) {
        if (!n) return nullptr;

        if      (key < n->key) n->left  = removeNode(n->left,  key);
        else if (key > n->key) n->right = removeNode(n->right, key);
        else {
            if (!n->left)  { Node* c = n->right; delete n; return c; }
            if (!n->right) { Node* c = n->left;  delete n; return c; }

            Node* s  = minNode(n->right);
            n->key   = s->key;
            n->value = s->value;
            n->right = removeNode(n->right, s->key);
        }

        return rebalance(n);
    }

    // select: return the node with 0-based rank k (0 = smallest)
    Node* selectNode(Node* n, int k) {
        if (!n) return nullptr;

        int leftSize = size(n->left);

        if      (k < leftSize) return selectNode(n->left,  k);
        else if (k > leftSize) return selectNode(n->right, k - leftSize - 1);
        else                   return n;   // this node is rank k
    }

    // rank: return the 0-based rank of the given key
    int rankOf(Node* n, Key key) {
        if (!n) return 0;

        if      (key < n->key) return rankOf(n->left, key);
        else if (key > n->key) return size(n->left) + 1 + rankOf(n->right, key);
        else                   return size(n->left);
    }

    // Collect the top N nodes (largest keys) via reverse in-order traversal
    void topN(Node* n, int n_count, std::vector<Value>& results) {
        if (!n || (int)results.size() >= n_count) return;
        topN(n->right, n_count, results);          // right first = largest first
        if ((int)results.size() < n_count)
            results.push_back(n->value);
        topN(n->left, n_count, results);
    }

    void destroy(Node* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

public:
    OSTTree() : root(nullptr) {}
    ~OSTTree() { destroy(root); }

    void insert(Key key, Value value) {
        root = insertNode(root, key, value);
    }

    void remove(Key key) {
        root = removeNode(root, key);
    }

    // Return the value at 0-based rank k (0 = smallest)
    // Returns false if k is out of range
    bool select(int k, Value& out) {
        Node* n = selectNode(root, k);
        if (!n) return false;
        out = n->value;
        return true;
    }

    // Return the 0-based rank of a key (-1 if not found)
    int rank(Key key) {
        return rankOf(root, key);
    }

    // Return up to n items with the largest keys (e.g. top N by rating)
    std::vector<Value> getTopN(int n) {
        std::vector<Value> results;
        results.reserve(n);
        topN(root, n, results);
        return results;
    }

    int totalSize() { return size(root); }
};
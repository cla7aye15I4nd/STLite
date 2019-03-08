#ifndef _map_hpp
#define _map_hpp

#include <functional>
#include <cstdio>
#include "error.hpp"
#include "algorithm.hpp"

namespace sakura{
    template <typename Key, typename Value, class Comparator = std::less<Key>>
    class map{
    public:
        static const bool RED = true;
        static const bool BLACK = false;
        
        using size_type = unsigned long;
        using value_type = pair<Key, Value>;

        #define key first
        #define value second
        
        map () : root(nullptr) {}
        
        struct node {
            value_type o;
            bool color;
            
            node *left, *right;
            
            node(Key key, Value value, bool color)
                : o(value_type(key, value)), color(color) {
                left = right = nullptr;
            }
        };
        
        using Node = node*;

        Value& get(Key key) { return get(root, key); }
        const Value& get(Key key) const{ return get(root, key); }
        
        void insert(Key key, Value value) {
            root = insert(root, key, value);
            root->color = BLACK;
        }

        void erase(Key key) {
            //printf("erase %d\n", key);
            root = erase(root, key);
        }

        void print() { print(root); }
        void print(Node u, int dep = 0) {
            if (u == nullptr) return;
            for (int i = 1; i <= dep; ++i) 
                putchar(' ');
            printf("|%d %s\n", u->o.key, !u->color ? "BLACK": "RED");
            // for (int i = 1; i <= dep; ++i) 
            //     putchar(' ');
            // puts("L");
            print(u->left, dep + 1);
            // for (int i = 1; i <= dep; ++i) 
            //     putchar(' ');
            // puts("R");
            print(u->right, dep + 1);
        }
        
    private:
        Node rotateLeft(Node h) {
            Node x = h->right;
            h->right = x->left;
            x->left = h;
            x->color = x->left->color;
            x->left->color = RED;
            return x;
        }
        
        Node rotateRight(Node h) {
            Node x = h->left;
            h->left = x->right;
            x->right = h;
            x->color = x->right->color;
            x->right->color = RED;
            return x;
        }
        
        void colorFlip(Node x) {
            x->color ^= 1;
            x->left->color ^= 1;
            x->right->color ^= 1;
        }
        
        bool isRed(Node x) {
            return x != nullptr && x->color == RED;
        }
        bool isBlack(Node x) {
            return x == nullptr || x->color == BLACK;
        }
        
        int compare(const Key& u, const Key& v) const{
            if (_compare(u, v)) return -1;
            if (_compare(v, u)) return +1;
            return 0;
        }
        
        Node newNode(const Key& key, const Value& value, bool color) {
            Node u = new node (key, value, color);
            return u;
        }

        Node fixUp(Node h) {
            if (isRed(h->right))
                h = rotateLeft(h);
            if (isRed(h->left) && isRed(h->left->left))
                h = rotateRight(h);
            if (isRed(h->left) && isRed(h->right))
                colorFlip(h);
            return h;
        }
        
        Node insert(Node x, const Key& key, const Value& val) {
            if (x == nullptr) return newNode(key, val, RED);
            int cmp = compare(key, x->o.key);

            if (cmp < 0) x->left = insert(x->left, key, val);
            else if (cmp > 0) x->right = insert(x->right, key, val);
            else x->o.value = val;
            
            if (isRed(x->right) && isBlack(x->left))
                x = rotateLeft(x);
            if (isRed(x->left) && isRed(x->left->left))
                x = rotateRight(x);
            if (isRed(x->left) && isRed(x->right))
                colorFlip(x);
            return x;
        }

        Node eraseMin(Node x) {
            if (x->left == nullptr)
                return nullptr;
            if (isBlack(x->left) && isBlack(x->left->left)) {
                colorFlip(x);
                if (isRed(x->right->left)) {
                    x->right = rotateRight(x->right);
                    x = rotateLeft(x);
                    colorFlip(x);
                }
            }
            x->left = eraseMin(x->left);
            return fixUp(x);
        }
        
        Node erase(Node x, const Key& key) {
            //printf("erase %d\n", key);
            int cmp = compare(key, x->o.key);
            if (cmp < 0) {
                if (isBlack(x->left) && isBlack(x->left->left)) {
                    colorFlip(x);
                    if (isRed(x->right->left)) {
                        x->right = rotateRight(x->right);
                        x = rotateLeft(x);
                        colorFlip(x);
                    }
                }
                x->left = erase(x->left, key);
            } else {
                if (isRed(x->left))
                    x = rotateRight(x);
                if (compare(key, x->o.key) == 0 && x->right == nullptr)
                    return nullptr;
                if (isBlack(x->right) && isBlack(x->right->left)) {
                    colorFlip(x);
                    if (isRed(x->left->left)) {
                        x = rotateRight(x);
                        colorFlip(x);
                    }
                }
                if (compare(key, x->o.key) == 0) {
                    Node h = x->right;
                    while (h->left != nullptr)
                        h = h->left;
                    x->o.key = h->o.key;
                    x->o.value = h->o.value;
                    eraseMin(x->right);
                }
                else x->right = erase(x->right, key);
            }
            
            return fixUp(x);
        }
        
        Node root;
        Comparator _compare;

        const Value& get(Node x, Key key) const{            
            while (x != nullptr) {
                int cmp = compare(key, x->key);
                if (cmp == 0) return x->value;
                if (cmp < 0) x = x->left;
                else x = x->right;
            }
            
        }

        Value& get(Node x, Key key) {
            while (x != nullptr) {
                int cmp = compare(key, x->o.key);
                if (cmp == 0) return x->o.value;
                if (cmp < 0) x = x->left;
                else x = x->right;
            }
        }
    };
}

namespace sjtu{
    using namespace sakura;
}

#endif

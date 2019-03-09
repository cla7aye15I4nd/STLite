#ifndef _map_hpp
#define _map_hpp

#include <functional>
#include <cstdio>
#include "error.hpp"
#include "algorithm.hpp"

namespace sakura{
    template <typename Key, typename Value, class Comparator = std::less<Key>>
    class map{
        static const bool RED = true;
        static const bool BLACK = false;
        
        using size_type = unsigned long;
        using value_type = pair<Key, Value>;

        class node;
        using Node = node*;
    public:

#define key first
#define value second
        
        map () : root(nullptr) {}
        ~map () { clear(); }
        
        void insert(Key key, Value value) {
            ++count;
            root = insert(root, key, value);
            root->color = BLACK;
        }
        
        void erase(Key key) {
            --count;
            root = erase(root, key);
            if (root != nullptr) root->color = BLACK;
        }

        Value& at(const Key &key) { return find(key)->o.value; }
	const Value& at(const Key &key) const { return find(key)->o.value; }
        Value& operator [] (Key key) { return find(key)->o.value; }
        const Value& operator [] (Key key) const{ return find(key)->o.value; }

        class const_iterator;
        class iterator{
        public:
            iterator() = default;
            iterator(const iterator &it) : id(it.id), x(it.x) {}
            
            iterator& operator++() { x = id->succ(x->o.key); return *this; }
            iterator& operator--() { x = x == nullptr ? id->max(id->root) : id->prev(x->o.key); return *this; }
            iterator operator++ (int) {
                iterator temp = *this;
                operator++ ();
                return temp;
            }
            iterator operator-- (int) {
                iterator temp = *this;
                operator-- ();
                return temp;
            }

            value_type& operator*() const{ return x->o; }
            value_type* operator->() const{ return &x->o; }
            bool operator== (const iterator &rhs) const { return id == rhs.id && x == rhs.x; }
            bool operator== (const const_iterator &rhs) const { return id == rhs.id && x == rhs.x; }
            bool operator!= (const iterator &rhs) const { return !(*this == rhs); }
            bool operator!= (const const_iterator &rhs) const { return !(*this == rhs); }
        private:
            const map* id;
            Node x;
        };

        class const_iterator {
        public:
            const_iterator() = default;
            const_iterator(const const_iterator &it) : id(it.id), x(it.x) {}
            const_iterator(const iterator &it) : id(it.id), x(it.x) {}
        private:
            const map* id;
            const Node x;
	};

        bool empty() const{ return count == 0; }
        size_type size() const{ return count; }
        
        void clear() { clear(root); }
        
    private:
        bool isRed(Node x)   const{ return x != nullptr && x->color == RED; }
        bool isBlack(Node x) const{ return x != nullptr && x->color == BLACK; }

        Node rotateLeft(Node h) const{
            Node x = h->right;
            h->right = x->left;
            x->left = h;
            x->color = x->left->color;
            x->left->color = RED;
            return x;
        }
        
        Node rotateRight(Node h) const{
            Node x = h->left;
            h->left = x->right;
            x->right = h;
            x->color = x->right->color;
            x->right->color = RED;
            return x;
        }

        void colorFlip(Node x) const{
            x->color ^= 1;
            x->left->color ^= 1;
            x->right->color ^= 1;
        }
        
        Node newNode(const Key& key, const Value& value, bool color) const{ return new node (key, value, color); }

        Node fixUp(Node h) const{
            if (isRed(h->right)) h = rotateLeft(h);
            if (isRed(h->left) && isRed(h->left->left)) h = rotateRight(h);
            if (isRed(h->left) && isRed(h->right)) colorFlip(h);
            return h;
        }
        
        Node insert(Node x, const Key& key, const Value& val) {
            if (x == nullptr) return temp = newNode(key, val, RED);

            if      (comp(key, x->o.key)) x->left  = insert(x->left,  key, val);
            else if (comp(x->o.key, key)) x->right = insert(x->right, key, val);
            
            return fixUp(x);
        }

        Node erase(Node x, const Key& key) const{
            if (comp(key, x->o.key)) {
                if (isBlack(x->left) && !isRed(x->left->left)) x = moveRedLeft(x);
                x->left = erase(x->left, key);
            } else {
                if (isRed(x->left)) x = rotateRight(x);
                if (isBlack(x->right) && !isRed(x->right->left)) x = moveRedRight(x);
                if (compare(key, x->o.key) == 0) {
                    if (x->right == nullptr) return nullptr;
                    else {
                        Node h = min(x->right);
                        x->o.key = h->o.key;
                        x->o.value = h->o.value;
                        x->right = eraseMin(x->right);
                    }
                }
                else x->right = erase(x->right, key);
            }
            
            return fixUp(x);
        }

        Node min(Node x) const{
            while (x->left != nullptr) x = x->left;
            return x;
        }

        Node max(Node x) const{
            while (x->right != nullptr) x = x->right;
            return x;
        }

        Node eraseMin(Node x) const{
            if (x->left == nullptr) return nullptr;
            if (isBlack(x->left) && !isRed(x->left->left)) x = moveRedLeft(x);
            x->left = eraseMin(x->left);
            return fixUp(x);
        }

        Node moveRedLeft(Node x) const{
            colorFlip(x);
            if (isRed(x->right->left)) {
                x->right = rotateRight(x->right);
                x = rotateLeft(x);
                colorFlip(x);
            }
            return x;
        }

        Node moveRedRight(Node x) const{
            colorFlip(x);
            if (isRed(x->left->left)) {
                x = rotateRight(x);
                colorFlip(x);
            }
            return x;
        }
        
        int compare(const Key& u, const Key& v) const{
            if (comp(u, v)) return -1;
            if (comp(v, u)) return +1;
            return 0;
        }

        Node prev(Node x, const Key& key) const{
            if (x == nullptr) return x;
            if (comp(x->key, key)) {
                Node h = prev(x->right, key);
                return h == nullptr ? x: h;
            }
            return prev(x->left, key);
        }

        Node succ(Node x, const Key& key) const{
            if (x == nullptr) return x;
            if (comp(key, x->key)) {
                Node h = prev(x->left, key);
                return h == nullptr ? x: h;
            }
            return prev(x->right, key);
        }

        Node succ(const Key& key) const{ return succ(root, key); }
        Node prev(const Key& key) const{ return prev(root, key); }

        Node _find(const Key& key) const{
            Node x = root;
            while (x != nullptr) {
                int cmp = compare(key, x->o.key);
                if (cmp == 0) return x;
                if (cmp < 0) x = x->left;
                else x = x->right;
            }
            return nullptr;
        }

        Node find(const Key& key) {
            Node x = _find(key);
            return x == nullptr ? insert(key, Value()), _find(key): x;
        }

        void clear(Node x) {
            if (x == nullptr) return;
            clear(x->left);
            clear(x->right);
            delete x;
        }

        class node {
        public:
            value_type o;
            node(Key key, Value value, bool color)
                : o(value_type(key, value)), color(color), left(nullptr), right(nullptr) {}
            
        private:
            bool color;
            node *left, *right;
            friend map;
        };

        
        Node root, temp;
        size_type count;
        Comparator comp;
        friend iterator;
    };
}

namespace sjtu{
    using namespace sakura;
}

#endif

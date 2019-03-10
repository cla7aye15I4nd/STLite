#ifndef _map_hpp
#define _map_hpp

#include <functional>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include "vector.hpp"
#include "error.hpp"
#include "algorithm.hpp"

namespace sakura{
    template <typename Key, typename Value, class Comparator = std::less<Key>>
    class map{
        static const bool RED = true;
        static const bool BLACK = false;

        class node;
        using Node = node*;
    public:
        using size_type = unsigned long;
        using value_type = pair<const Key, Value>;

#define key first
#define value second
        
        map () : root(nullptr), counter(0) {}
        map (const map& mp) : root(nullptr) { copy(mp); }
        ~map () { clear(); }

        Value& at(const Key &key) {
            Node x = _get(key);
            if (x == nullptr) throw index_out_of_bound("at");
            return x->value;
        }
	const Value& at(const Key &key) const {
            Node x = _get(key);
            if (x == nullptr) throw index_out_of_bound("at");
            return x->value;
        }
        Value& operator [] (Key key) { return get(key)->value; }
        const Value& operator [] (Key key) const{
            Node x = _get(key);
            if (x == nullptr) throw index_out_of_bound("at");
            return x->value;
        }

        map& operator= (const map& mp) { if (this != &mp) copy(mp); return *this; }
        
        class const_iterator;
        class iterator{
        public:
            iterator() = default;
            iterator(const map *id, Node x) : id(id), x(x) {}
            iterator(const iterator &it) : id(it.id), x(it.x) {}

            iterator& operator= (const iterator &it) {
                id = it.id; x = it.x;
                return *this;
            }
            iterator& operator++() {
                if (x == nullptr) throw invalid_iterator("++");
                x = id->succ(x->key);
                return *this;
            }
            iterator& operator--() {
                x = (x == nullptr ? id->max(id->root) : id->prev(x->key));
                if (x == nullptr) throw invalid_iterator("--");
                return *this;
            }
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

            node& operator*() const{ return *x; }
            Node operator->() const{ return x; }
            bool operator== (const iterator &rhs) const { return id == rhs.id && x == rhs.x; }
            bool operator== (const const_iterator &rhs) const { return id == rhs.id && x == rhs.x; }
            bool operator!= (const iterator &rhs) const { return !(*this == rhs); }
            bool operator!= (const const_iterator &rhs) const { return !(*this == rhs); }
        private:
            const map* id;
            Node x;
            friend map;
        };

        class const_iterator : public iterator{
        public:
            const_iterator() = default;
            const_iterator(const map *id, Node x) : iterator(id, x) {}
            const_iterator(const iterator &it) : iterator(it) {}
            const_iterator(const const_iterator &it) : iterator(it) {}
        };

        pair<iterator, bool> insert(const value_type &o) {
            size_type c = counter;
            root = _insert(root, o.key, o.value);
            return pair<iterator, bool>(iterator(this, temp), counter - c);
        }
        
        void erase(const iterator &it) {
            if (it.x == nullptr || it.id != this || _get(it.x->key) == nullptr)
                throw invalid_iterator("erase");
            root = _erase(root, it.x->key);
            if (root != nullptr) root->color = BLACK;
        }

        size_t count(const Key &key) const{ return _get(key) != nullptr; }
        iterator find(const Key &key) { return iterator(this, _get(key)); }
	const_iterator find(const Key &key) const { return const_iterator(this, _get(key)); }

        bool empty() const{ return counter == 0; }
        size_type size() const{ return counter; }
        
        void clear() {
            for (size_type i = 0; i < pool.size(); ++i)
                delete pool[i];
            pool.clear();
            counter = 0;
            root = nullptr;
        }

        iterator begin() { return iterator(this, counter == 0 ? nullptr: min(root)); }
        iterator end()   { return iterator(this, nullptr); }
        const_iterator cbegin() const{ return const_iterator(this, min(root)); }
        const_iterator cend()   const{ return const_iterator(this, nullptr); }
        
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
        
        Node fixUp(Node h) const{
            if (isRed(h->right)) h = rotateLeft(h);
            if (isRed(h->left) && isRed(h->left->left)) h = rotateRight(h);
            if (isRed(h->left) && isRed(h->right)) colorFlip(h);
            return h;
        }
        
        Node _insert(Node x, const Key& key, const Value& val) {
            if (x == nullptr) {
                ++counter;
                temp = new node (key, val, RED);
                pool.push_back(temp);
                return temp;
            }
            if      (comp(key, x->key)) x->left  = _insert(x->left,  key, val);
            else if (comp(x->key, key)) x->right = _insert(x->right, key, val);
            else    temp = x;
            return fixUp(x);
        }

        Node _insert(Node x, const Key& key) {
            if (x == nullptr) {
                temp = new node(key, RED);
                pool.push_back(temp);
                return temp;
            }
            if      (comp(key, x->key)) x->left  = _insert(x->left,  key);
            else if (comp(x->key, key)) x->right = _insert(x->right, key);
            return fixUp(x);
        }
        
        Node _erase(Node x, const Key& key) {
            if (comp(key, x->key)) {
                if (isBlack(x->left) && !isRed(x->left->left)) x = moveRedLeft(x);
                x->left = _erase(x->left, key);
            } else {
                if (isRed(x->left)) x = rotateRight(x);
                if (isBlack(x->right) && !isRed(x->right->left)) x = moveRedRight(x);
                if (compare(key, x->key) == 0) {
                    --counter;
                    if (x->right == nullptr) {
                        return nullptr;
                    }else {
                        Node h = min(x->right);
                        memcpy(&x->key, &h->key, sizeof(Key));
                        new(&x->value) Value(h->value);
                        x->right = eraseMin(x->right);
                    }
                }
                else x->right = _erase(x->right, key);
            }
            
            return fixUp(x);
        }

        Node min(Node x) const{
            if (x != nullptr) while (x->left != nullptr) x = x->left;
            return x;
        }

        Node max(Node x) const{
            if (x != nullptr) while (x->right != nullptr) x = x->right;
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
                Node h = succ(x->left, key);
                return h == nullptr ? x: h;
            }
            return succ(x->right, key);
        }

        Node succ(const Key& key) const{ return succ(root, key); }
        Node prev(const Key& key) const{ return prev(root, key); }

        Node _get(const Key& key) const{
            Node x = root;
            while (x != nullptr) {
                int cmp = compare(key, x->key);
                if (cmp == 0) return x;
                if (cmp < 0) x = x->left;
                else x = x->right;
            }
            return nullptr;
        }

        Node get(const Key& key) {
            Node x = _get(key);
            if (x == nullptr) {
                ++counter;
                root = _insert(root, key);
                return temp;
            }
            else return x;
        }
        
        void copy(const map& mp) {
            clear();
            counter = 0;
            copy(mp.root);
        }

        class node {
        public:
            Key key;
            Value value;
            node(Key key, Value value, bool color) : key(key), value(value), color(color), left(nullptr), right(nullptr) {}
            node(Key key, bool color) : key(key), color(color), left(nullptr), right(nullptr) {}
            
        private:
            bool color;
            node *left, *right;
            friend map;
        };

        void copy(Node rt) {
            if (rt == nullptr) return;
            insert(value_type(rt->key, rt->value));
            copy(rt->left); copy(rt->right);
        }
        
        Node root, temp;
        size_type counter;
        Comparator comp;
        vector<Node> pool;
        friend iterator;
        #undef key
        #undef value
    };
}

namespace sjtu{ using namespace sakura; }

#endif

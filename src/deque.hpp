#ifndef _deque_hpp
#define _deque_hpp

#include "error.hpp"
#include "vector.hpp"
#include <cstddef>

namespace sakura{
    template <typename _Tp>
    class deque{
        class node;
        using size_type = unsigned long;
        using Node = node*;
        
    public:
        deque () : null(new node), root(null) {
            pool.push_back(null);
            null->left  = null;
            null->right = null;
            null->parent = null;
            null->size  = 0;
        }

        virtual ~deque () { clear(); }
        _Tp& find(size_type kth) const{
            Node u = root;
            while (true) {
                if (kth <= u->left->size) u = u->left;
                else if (kth > u->left->size + 1) {
                    kth -= u->left->size + 1;
                    u = u->right;
                }
                else return u->data;
            }
        }

        size_type size() const{ return root->size; }
        bool empty() const{ return root == null; }
        
        const _Tp& front() const{ return find(1); }
        const _Tp& back() const{ return find(root->size); }
        
        void pop_front() { erase(1); }
        void pop_back() { erase(root->size); }
        void push_front(const _Tp &data) { insert(root, 0, data); }        
        void push_back(const _Tp &data) { insert(root, root->size, data); }
        void insert(size_type kth, const _Tp &data) { insert(root, kth, data); }
        void erase(size_type kth) { erase(root, kth); }

        _Tp& at(size_type index) { return find(index); }
        const _Tp& at(size_type index) const{ return find(index); }
        _Tp& operator[] (size_type index) { return find(index); }
        const _Tp& operator[] (size_type index) const{ return find(index); }

        class iterator {
        public:
            iterator operator+ (const int &n) const {
                
            }
            
            iterator operator-(const int &n) const {
            }
            int operator-(const iterator &rhs) const {
            }
            iterator operator+=(const int &n) {
            }
            iterator operator-=(const int &n) {}
            iterator operator++(int) {}
            iterator& operator++() {}
            iterator operator--(int) {}
            iterator& operator--() {}
            T& operator*() const {}
            T* operator->() const noexcept {}

            bool operator==(const iterator &rhs) const {}
            bool operator==(const const_iterator &rhs) const {}
            bool operator!=(const iterator &rhs) const {}
            bool operator!=(const const_iterator &rhs) const {}
        private:
            Node ptr;
        };
        
    private:
        void insert(Node &h, size_type kth, const _Tp &data) {
            if (h == null) h = newNode(data);
            else {
                h->size ++;
                if (h->left->size < kth) {
                    insert(h->right, kth - h->left->size - 1, data);
                    fixUpRight(h);
                } else {
                    insert(h->left, kth, data);
                    fixUpLeft(h);
                }
            }
        }

        void erase(Node &h, size_type kth) {
            h->size--;
            if (h->left->size >= kth) return erase(h->left, kth);
            if (kth > h->left->size + 1) return erase(h->right, kth-h->left->size-1);

            if (h->right == null) h = h->left;
            else if (h->left == null) h = h->right;
            else if (h->right->left == null) {
                h = h->right;
                h->size += h->left->size;
            }else {
                Node u = h->right;
                while (u->left->left != null) {
                    u->size--;
                    u = u->left;
                }

                u->left->size = h->size;
                h = u->left;
                u->size--;
                u->left = null;
            }
        }
        
        Node newNode(const _Tp &data) {
            Node h = new node;
            h->left  = null;
            h->right = null;
            h->data  = data;
            h->size  = 1;
            pool.push_back(h);
            return h;
        }
        
        void rotateLeft(Node &h) {
            Node x = h->right;
            h->right = x->left;
            x->left = h;            
            x->size = h->size;
            h->size = h->left->size + h->right->size + 1;
            h = x;
        }
        
        void rotateRight(Node &h) {
            Node x = h->left;
            h->left = x->right;
            x->right = h;
            x->size = h->size;
            h->size = h->left->size + h->right->size + 1;
            h = x;
        }
        
        bool fixLeft(Node& h) {
            if (h->left->left->size > h->right->size) {
                rotateRight(h);
                return true;
            }
            if (h->left->right->size > h->right->size) {
                rotateLeft(h->left);
                rotateRight(h);
                return true;
            }
            return false;
        }
        
        bool fixRight(Node& h) {
            if (h->right->right->size > h->left->size) {
                rotateLeft(h);
                return true;
            }
            if (h->right->left->size > h->left->size) {
                rotateRight(h->right);
                rotateLeft(h);
                return true;
            }
            return false;
        }

        void fixUpLeft(Node &h) {
            if (h == null) return;
            if (fixLeft(h))
                fixUp(h);
        }

        void fixUpRight(Node &h) {
            if (h == null) return;
            if (fixRight(h))
                fixUp(h);
        }

        void fixUp(Node &h) {
            fixUpLeft(h->left);
            fixUpRight(h->right);
            fixUpLeft(h);
            fixUpRight(h);
        }

        void clear() {
            for (auto u : pool)
                delete u;
        }

        size_type rank(Node u) {
            size_type retval = 1 + u->left->size;
            
        }

        vector<Node> pool;
        Node null, root;
        
        class node{
        public:
            _Tp data;
        private:
            size_type size;
            Node left, right, parent;
            friend deque;
        };
    };        
}

#endif

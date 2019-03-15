#ifndef _priority_queue_hpp
#define _priority_queue_hpp

#include "exceptions.hpp"
#include <functional>

namespace sjtu{
    template <typename _Tp, class Comparator = std::less<_Tp>>
    class priority_queue{
        using size_type = unsigned long;
    
    public:
        priority_queue () : root(nullptr), count(0) {}
        priority_queue (const _Tp& elem) : root(node(elem)), count(0) {}
        priority_queue (const priority_queue& q) : root(nullptr), count(0) { copy(q); }
        virtual ~priority_queue () { clear(); }
        
        priority_queue& operator= (const priority_queue& q) {
            if (this != &q) 
                copy(q);
            return *this;
        }
        
        void merge(priority_queue& u) {
            count += u.count;
            root = _merge(root, u.root);
            u.root = nullptr;
            u.count = 0;
        }
  
        void push(const _Tp &elem) {
            ++count;
            root = _merge(root, new node(elem));
        }

        void pop() {
            if (count == 0) throw container_is_empty();
            --count;
            node* temp = root;
            root = merge_pairs(root->head);
            delete temp;
        }

        bool empty() const{
            return count == 0;
        }
        size_type size() const{
            return count;
        }
        const _Tp& top() const{
            if (count == 0) throw container_is_empty();
            return root->elem;
        }
  
    private:
        class node{
        public:
            node () : head(nullptr), next(nullptr) {}
            node (const _Tp& elem_) : elem(elem_), head(nullptr), next(nullptr) {}
            virtual ~node () {}
            
            friend priority_queue;
        private:
            void addChild(node* u) {
                if (head == nullptr) 
                    head = u;
                else {
                    u->next = head;
                    head = u;
                }
            }
    
            _Tp elem;
            node *head, *next;
        };
        
        node* _merge(node* u, node* v) {
            if (u == nullptr) return v;
            if (v == nullptr) return u;
    
            if (compare(u->elem, v->elem)) {
                v->addChild(u);
                return v;
            } else {
                u->addChild(v);
                return u;
            }
        }

        node* merge_pairs(node* u) {
            // node* v = u;
            // u = u->next;
            // v->next = nullptr;
            // while (u->next != nullptr) {
            //     node *a = u->next, *b = a->next;
            //     u->next = a->next = nullptr;
            //     v = _merge(v, _merge(u, a));
            //     u = b;
            // }

            // return _merge(v, u);
            if (u == nullptr) return nullptr;
            if (u->next == nullptr) return u;
            
            node *t1 = u->next, *t2 = u->next->next;
            u->next->next = nullptr;
            u->next = nullptr;
            return _merge(_merge(u, t1), merge_pairs(t2));
        }
        
        void clear_(node *u) {
            if (u == nullptr) return;
            for (node *v = u->head; v; v = v->next)
                clear_(v);
            delete u;
        }

        void clear() {
            clear_(root);
            count = 0; root = nullptr;
        }
        
        void copy_(node *src) {
            if (src == nullptr) return;
            push(src->elem);
            for (node *v = src->head; v; v = v->next)
                copy_(v);
        }
        
        void copy(const priority_queue& q) {
            clear();
            copy_(q.root);
        }
    
        node* root;
        Comparator compare;
        size_type count;
    };
}

#endif

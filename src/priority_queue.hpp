#ifndef _priority_queue_hpp
#define _priority_queue_hpp

#include "error.hpp"
#include <functional>

namespace sakura{
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
            if (count == 0) throw container_is_empty("pop");
            --count;
            root = merge_pairs(root->head);
        }

        bool empty() const{
            return count == 0;
        }
        size_type size() const{
            return count;
        }
        const _Tp& top() const{
            if (count == 0) throw container_is_empty("top");
            return root->elem;
        }
  
    private:
        class node{
        public:
            node () : elem(_Tp()), head(nullptr), next(nullptr) {}
            node (const _Tp& elem_) : elem(elem_), head(nullptr), next(nullptr) {}
            
            friend priority_queue;
        private:
            void addChild(node* u) {
                if (head == nullptr) {
                    head = u;
                } else {
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
    
            if (!Comparator () (u->elem, v->elem)) {
                u->addChild(v);
                return u;
            } else {
                v->addChild(u);
                return v;
            }
        }

        node* merge_pairs(node* u) {
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
        size_type count;
    };
}

namespace sjtu{
    using namespace sakura;
}

#endif

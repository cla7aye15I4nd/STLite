#pragma GCC optimize(3)
#ifndef _deque_hpp
#define _deque_hpp
 
#include <cstddef>
 
#include "utility.hpp"
#include "exceptions.hpp"
 
namespace sjtu{
    template <typename _Tp>
    class deque {
#define HEAD s.head->next
#define TAIL s.tail->prev
#define END  s.tail
        static const size_t SIZE = 400;
        static const size_t BOUND = SIZE * 2;
	static const size_t LIMIT = SIZE / 2;
 
        template <typename __Tp>
        class list{
        public:
            class node;
            using Node = node*;
            class node{
            public:
                node (Node _prev = nullptr, Node _next = nullptr, __Tp *_data = nullptr)
                    : prev(_prev), next(_next), data(_data) {}
                virtual ~node() { if (data != nullptr) delete data; }
            
                Node prev, next;
                __Tp *data;
            };
 
            virtual ~list () { clearAll(); }
        
            list () :count(0)  {
                head = new node ();
                tail = new node ();
                head->next = tail;
                tail->prev = head;
            }
 
            list (const list& other) { copy(other); }
            list& operator= (const list& other) {
                if (this != &other) {
                    clearAll();
                    copy(other);
                }
                return *this;
            }
        
            void push_back(const __Tp& data) {
                ++count;
                Node u = new node (tail->prev, tail, new __Tp(data));
                tail->prev->next = u;
                tail->prev = u;
            }
 
            void push_front(const __Tp& data) {
                ++count;
                Node u = new node (head, head->next, new __Tp(data));
                head->next->prev = u;
                head->next = u;
            }
 
            void push_back(__Tp *data) {
                ++count;
                Node u = new node (tail->prev, tail, data);
                tail->prev->next = u;
                tail->prev = u;
            }
 
            void pop_front() {
                --count;
                Node temp = head->next;
                head->next->next->prev = head;
                head->next = head->next->next;
                delete temp;
            }
 
            void pop_back() {
                --count;
                Node temp = tail->prev;
                tail->prev->prev->next = tail;
                tail->prev = tail->prev->prev;
                delete temp;
            }
            
            void insert(Node u, __Tp data) {
                ++count;
                Node temp = new node (u->prev, u, new __Tp (data));
                u->prev->next = temp;
                u->prev = temp;
            }
            
            void insert(Node u, __Tp *data) {
                ++count;
                Node temp = new node (u->prev, u, data);
                u->prev->next = temp;
                u->prev = temp;
            }
 
            void erase(Node u) {
                --count;
                u->prev->next = u->next;
                u->next->prev = u->prev;
                delete u;
            }
 
            const __Tp& front() const{ return *head->next->data; }
            const __Tp& back()  const{ return *tail->prev->data; }
            
            void _erase_tail() {
                if (tail != nullptr) 
                    delete tail;
            }
            void _erase_head() {
                if (head != nullptr) 
                    delete head;
            }
        
            void merge(list *u) {
                count += u->count;
                u->count = 0;
                tail->prev->next = u->head->next;
                u->head->next->prev = tail->prev;
                u->tail->prev->next = tail;
                tail->prev = u->tail->prev;
                u->head->next = u->tail;
                u->tail->prev = u->head;
            }
        
            list* split(size_t pos) {
                Node x = _find(pos);
                Node _head = new node(nullptr, x->next);
                Node _tail = new node(tail->prev, nullptr);
                list* retval = new list (count - pos, _head, _tail);
                x->next->prev = _head;
                tail->prev->next = _tail;
                tail->prev = x;
                x->next = tail;
                count = pos;
                return retval;
            }
 
            Node _find(size_t pos) {
                Node x = head;
                while (pos--) x = x->next;
                return x;
            }
 
            list (size_t count, Node head, Node tail)
                :count(count), head(head), tail(tail) {}
 
            void clear() {
                count = 0;
                Node x = head->next;
                while (x != tail) {
                    Node y = x->next;
                    erase(x);
                    x = y;
                }
            }
 
            void clearAll() {
                clear();
                _erase_head();
                _erase_tail();
            }
 
            void copy(const list& other) {
                head = new node ();
                tail = new node ();
                head->next = tail;
                tail->prev = head;
                if (other.head->data != nullptr) head->data = new __Tp (*other.head->data);
                if (other.tail->data != nullptr) tail->data = new __Tp (*other.tail->data);
                auto ptr = other.head->next;
                while (ptr != other.tail) {
                    insert(tail, new __Tp (*ptr->data));
                    ptr = ptr->next;
                }
                count = other.count;
            }
            
            size_t count;
            Node head, tail;
        };
        
        using List = typename list<list<_Tp>>::Node;
        using Elem = typename list<_Tp>::Node;
    public:
        deque () : count(0) {
            srand(300000);
            s.push_back(new list<_Tp> ());
            END->data = new list<_Tp> ();
            END->data->count = 1;
        }
 
        deque (const deque& other) {
            s = other.s;
            count = other.count;
        }
 
        deque& operator= (const deque& other) {
            if (this != &other) {
                s = other.s;
                count = other.count;
            }
            return *this;
        }
        
        void push_back(const _Tp& data) { ++count; TAIL->data->push_back(data); maintain(TAIL); }
        void push_front(const _Tp& data) { ++count; HEAD->data->push_front(data);  maintain(HEAD); }
        void pop_back() { --count; TAIL->data->pop_back();  maintain(TAIL); }
        void pop_front() { --count; HEAD->data->pop_front(); maintain(HEAD); }
        const _Tp& front() const{ if (count == 0) throw index_out_of_bound(); return s.front().front(); }
        const _Tp& back() const{ if (count == 0) throw index_out_of_bound(); return s.back().back(); }
        bool empty() const{ return count == 0; }
        size_t size() const{ return count; }
        
        class iterator{
        public:
            iterator () {}
            iterator& operator+ (int n) {
                if (n < 0) return operator- (-n);
                int r = rest();
                if (r >= n) move_back_ptr(n);
                else {
                    id = id->next;
                    move_back_ptr(move_back_block(n - r));
                }
                return *this;
            }
            iterator& operator- (int n) {
                if (n < 0) return operator+ (-n);
                if (index_ptr() <= n) throw invalid_iterator();
                int r = index() - 1;
                if (r >= n) move_front_ptr(n);
                else {
                    id = id->prev;
                    move_front_ptr(move_front_block(n - r));
                }
                return *this;
            }
            iterator& operator+= (int n) { return *this = *this + n; }
            iterator& operator-= (int n) { return *this = *this - n; }
            iterator& operator++ () { return operator+ (1); }
            iterator& operator-- () { return operator- (1); }
            iterator operator++ (int) {
                iterator temp = *this;
                operator++();
                return temp;
            }
            iterator operator-- (int) {
                iterator temp = *this;
                operator--();
                return temp;
            }
 
            int operator- (const iterator& rhs) {
                if (pid != rhs.pid) throw invalid_iterator();
                return index_ptr() - rhs.index_ptr();
            }
 
            bool operator== (const iterator &rhs) const {
                if (pid == rhs.pid) {
                    if (rhs.id->data->count == 0 || id->data->count == 0) return true;
                    return id == rhs.id && elem == rhs.elem;
                }
                return false;
            }
            bool operator!= (const iterator &rhs) const { return  !(*this == rhs); }
            
            _Tp& operator*()  const {
                if (id->next == nullptr) throw index_out_of_bound();
                return *(elem->data);
            }
            _Tp* operator->() const { return elem->data; }
            
        private:
            iterator (const deque* _pid, List _id, Elem _elem) : pid(_pid), id(_id), elem(_elem) {}
 
            int index() const{
                Elem x = id->data->head;
                int retval = 0;
                while (x != elem) {
                    x = x->next;
                    retval++;
                }
                return retval;
            }
            int rest() const{ return id->data->count - index(); }
            void move_back_ptr(int shift) {
                while (elem->next != nullptr && shift--) elem = elem->next;
                if (elem->next == nullptr && shift) throw invalid_iterator();
            }
            size_t move_back_block(size_t shift) {
                if (id == nullptr)  throw invalid_iterator();
                while (id->next != nullptr && shift > id->data->count) {
                    shift -= id->data->count;
                    id = id->next;
                }
                elem = id->data->head;
                return shift;
            }
            void move_front_ptr(size_t shift) { while (shift--) elem = elem->prev; }
            size_t move_front_block(size_t shift) {
                while (shift > id->data->count) {
                    shift -= id->data->count;
                    id = id->prev;
                }
                elem = id->data->tail;
                return shift;
            }
            int index_ptr() const{
                auto tid = id;
                int retval = -rest();
                while (tid->data != nullptr) {
                    retval += tid->data->count;
                    tid = tid->prev;
                }
                return retval;
            }
 
 
            const deque* pid;
            List id;
            Elem elem;
            friend deque;
        };
 
        using const_iterator = iterator;
 
        iterator begin() const{ return iterator(this, HEAD, HEAD->data->head->next); }
        iterator end()   const{ return iterator(this, END, END->data->tail); }
        const_iterator cbegin() const{ return const_iterator(this, HEAD, HEAD->data->head->next); }
        const_iterator cend()   const{ return const_iterator(this, END, END->data->tail); }
 
        iterator insert(iterator pos, const _Tp &data) {
            if (pos.pid != this) throw invalid_iterator();
            if (pos == end())
                pos = iterator(this, TAIL, TAIL->data->tail);
            ++count;
            size_t index = pos.index();
            pos.id->data->insert(pos.elem - 0, data);
            if (pos.id->data->count >= BOUND) {
                s.insert(pos.id->next, pos.id->data->split(SIZE));    
                return iterator(this, pos.id, pos.id->data->head->next) + (index - 1);
            }
            return pos - 1;
        }
 
        iterator erase(iterator pos) {
            if (pos.pid != this || pos == end()) throw invalid_iterator();
            --count;
            int index = pos.index_ptr();
            pos.id->data->erase(pos.elem);
            maintain(pos.id);
            return begin() + (index - 1);
        }
 
        _Tp& at(const size_t &pos) {
            if (pos > count) throw index_out_of_bound();
            return *(begin() + pos);
        }
	const _Tp& at(const size_t &pos) const {
            if (pos > count) throw index_out_of_bound();
            return *(begin() + pos);
        }
	_Tp& operator[](const size_t &pos) {
            if (pos > count) throw index_out_of_bound();
            return *(begin() + pos);
        }
	const _Tp& operator[](const size_t &pos) const {
            if (pos > count) throw index_out_of_bound();
            return *(begin() + pos);
        }
 
        void clear() {
            count = 0;
            s.clear();
            s.push_back(new list<_Tp> ());
        }
        
        virtual ~deque() {}
        
    private:
        void maintain(List x) {
            if (x->data->count >= BOUND) {
                s.insert(x->next, x->data->split(SIZE));
            }else if (x->prev != s.head || x->next != END) {
                if (x->next != END && (x->data->count == 0 || x->data->count + x->next->data->count <= SIZE)) {
                    x->data->merge(x->next->data);
                    s.erase(x->next);
                }
                if (x->prev != s.head && (x->data->count == 0 || x->data->count + x->prev->data->count <= SIZE)) {
                    x = x->prev;
                    x->data->merge(x->next->data);
                    s.erase(x->next);
                }
            }
        }
        
        list<list<_Tp>> s;
        size_t count;
#undef HEAD
#undef TAIL
#undef END
    };
}
 
#endif

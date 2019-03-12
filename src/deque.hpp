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
        static const size_t BOUND = 2048;
	static const size_t LIMIT = BOUND / 2;

        template <typename __Tp>
        class list{
        public:
            class node;
            using Node = node*;
            class node{
            public:
                node (Node _prev = nullptr, Node _next = nullptr, __Tp *_data = nullptr)
                    : prev(_prev), next(_next), data(_data) {}
                virtual ~node() {
                    if (data != nullptr) delete data;
                }
            
                Node prev, next;
                __Tp *data;
            };

            virtual ~list () {
                // clear();
                // _erase_head();
                // _erase_tail();
            }
        
            list () :count(0)  {
                head = new node ();
                tail = new node ();
                head->next = tail;
                tail->prev = head;
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

            void erase(Node u) {
                --count;
                u->prev->next = u->next;
                u->next->prev = u->prev;
                delete u;
            }

            const __Tp& front() const{ return *head->next->data; }
            const __Tp& back()  const{ return *tail->prev->data; }
            
            void _erase_tail() {
                if (tail != nullptr) {
                    delete tail;
                    tail = nullptr;
                }
            }
            void _erase_head() {
                if (head != nullptr) {
                    delete head;
                    head = nullptr;
                }
            }
        
            void merge(list *u) {
                count += u->count;
                tail->prev->next = u->head->next;
                u->head->next->prev = tail->prev;
                _erase_tail();
                u->_erase_head();
                tail = u->tail;
                u->tail = nullptr;
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
                Node x = head->next;
                while (x != tail) {
                    Node y = x->next;
                    erase(x);
                    x = y;
                }
            }
            
            size_t count;
            Node head, tail;
        };
        
        using List = typename list<list<_Tp>>::Node;
        using Elem = typename list<_Tp>::Node;
    public:
        deque () : count(0) {
            s.push_back(list<_Tp> ());
            END->data = new list<_Tp> (0, new typename list<_Tp>::node (), nullptr);
        }
        
        void push_back(const _Tp& data) { ++count; TAIL->data->push_back(data); maintain(TAIL); }
        void push_front(const _Tp& data) { ++count; HEAD->data->push_front(data);  maintain(HEAD); }
        void pop_back() { --count; TAIL->data->pop_back();  maintain(TAIL); }
        void pop_front() { --count; HEAD->data->pop_front(); maintain(HEAD); }
        const _Tp& front() const{ return s.front().front(); }
        const _Tp& back() const{ return s.back().back(); }
        bool empty() const{ return count == 0; }
        size_t size() const{ return count; }
        
        class const_iterator;
        class iterator{
        public:
            iterator () {}
            iterator operator+ (size_t n) {
                size_t r = rest();
                if (r >= n) move_back_ptr(n);
                else {
                    id = id->next;
                    move_back_ptr(move_back_block(n - r));
                }
                return *this;
            }
            iterator operator- (size_t n) {
                if (id->data->tail == nullptr) {
                    id = id->prev;
                    elem = id->data->tail;
                }
                size_t r = index();
                if (r >= n) move_front_ptr(n);
                else {
                    id = id->prev;
                    move_front_ptr(move_front_block(n - r));
                }
                return *this;
            }
            iterator operator+= (size_t n) { return *this = *this + n; }
            iterator operator-= (size_t n) { return *this = *this - n; }
            iterator operator++ () { return operator+ (1); }
            iterator operator-- () { return operator- (1); }
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

            size_t operator- (const iterator& rhs) { return index_ptr() - rhs.index_ptr(); }

            bool operator== (const iterator &rhs) const { return id == rhs.id && elem == rhs.elem; }
            bool operator== (const const_iterator &rhs) const { return id == rhs.id && elem == rhs.elem; }
            bool operator!= (const iterator &rhs) const { return  !(*this == rhs); }
            bool operator!= (const const_iterator &rhs) const { return !(*this == rhs); }

            _Tp& operator*()  const { return *(elem->data); }
            _Tp* operator->() const { return elem->data; }
            
        private:
            iterator (List _id, Elem _elem) : id(_id), elem(_elem) {}

            size_t index() const{
                Elem x = id->data->head;
                size_t retval = 0;
                while (x != elem) {
                    x = x->next;
                    retval++;
                }
                return retval;
            }
            size_t rest() const{ return id->data->count - index(); }
            void move_back_ptr(size_t shift) {
                while (elem->next != nullptr && shift--) elem = elem->next;
            }
            size_t move_back_block(size_t shift) {
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
            size_t index_ptr() const{
                auto tid = id;
                size_t retval = -rest();
                while (tid->data != nullptr) {
                    retval += tid->data->count;
                    tid = tid->prev;
                }
                return retval;
            }

            
            List id;
            Elem elem;
            size_t count;
            friend deque;
        };

        iterator begin() const{ return iterator(HEAD, HEAD->data->head->next); }
        iterator end()   const{ return iterator(END, END->data->head); }

        iterator insert(iterator pos, const _Tp &data) {
            ++count; pos -= 0;
            size_t index = pos.index_ptr();
            pos.id->data->insert(pos.elem - 0, data);
            maintain(pos.id);
            return begin() + index;
        }

        iterator erase(iterator pos) {
            --count;
            size_t index = pos.index_ptr();
            pos.id->data->erase(pos.elem);
            maintain(pos.id);
            return begin() + index;
        }

        _Tp& at(const size_t &pos) { return *(begin() + pos); }
	const _Tp& at(const size_t &pos) const { return *(begin() + pos); }
	_Tp& operator[](const size_t &pos) { return *(begin() + pos); }
	const _Tp& operator[](const size_t &pos) const { return *(begin() + pos); }

        void clear() { count = 0; s.clear(); }
        virtual ~deque() {}
        
    private:
        void maintain(List x) {
            if (x->data->count >= BOUND) {
                list<_Tp> y = *x->data->split(LIMIT);
                s.insert(x->next, y);
            }else if (x->next != END && x->data->count + x->next->data->count <= BOUND) {
                x->data->merge(x->next->data);
                s.erase(x->next);
            }else if (x->prev != s.head && x->data->count + x->prev->data->count <= BOUND) {
                x->prev->data->merge(x->data);
                s.erase(x);
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

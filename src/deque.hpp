#ifndef _deque_hpp
#define _deque_hpp

#include "error.hpp"
#include "vector.hpp"

namespace sakura{
    template <typename _Tp>
    class deque{
        using size_type = unsigned int;
        static const size_type BLOCK_SIZE = 1 << 10;
        static const size_type LOW_BOUND  = BLOCK_SIZE >> 1;
        static const size_type HIGH_BOUND = BLOCK_SIZE << 1;
        class block;
        
    public:
        deque () : head(new block()), count(0) {}

        const _Tp& front() const{ return *find(0); }
        const _Tp& back() const{ return *find(count-1); }
        
        block* insert(size_type index, const _Tp& value) {
            ++count;
            block *ptr = find_block(index);
            ptr->insert(index, value);
            if (ptr->size() == HIGH_BOUND)
                ptr = split(ptr);
            return ptr;
        }

        void erase(size_type index) {
            --count;
            block *ptr = find_block(index);
            ptr->erase(index);
            while (ptr->next != nullptr && ptr->next->size() + ptr->size() < BLOCK_SIZE)
                merge(ptr, ptr->next);
        }
        
        void push_back(const _Tp& value) { insert(count, value); }
        void push_front(const _Tp& value) { insert(0, value); }
        void pop_back() { erase(count - 1); }
        void pop_front() { erase(0); }

        _Tp& operator [] (size_type index) { return *find(index); }
        const _Tp& operator [] (size_type index) const{ return *find(index); }

        size_type size() const{ return count; }
        bool empty() const{ return count == 0; }

    private:
        class block : private vector<_Tp> {
        public:
            block () : prev(nullptr), next(nullptr) {
                this->reserve (BLOCK_SIZE);
            }

            virtual ~block() {}
            
            bool full() const{ return this->size() == BLOCK_SIZE; }

            friend deque;
        private:
            block *prev, *next;
        };

        void expand_front() {
            while (head->prev != nullptr)
                head = head->prev;
        }

        block* split(block *ptr) {
            block *temp = new block ();
            for (size_type i = BLOCK_SIZE; i < ptr->size(); ++i)
                temp->push_back(ptr->at(i));
            ptr->resize(BLOCK_SIZE);

            temp->next = ptr->next;
            temp->prev = ptr;

            if (ptr->next != nullptr)
                ptr->next->prev = temp;
            ptr->next = temp;
            return temp;
        }

        void merge(block *x, block *y) {
            for (size_type i = 0; i < y->size(); ++i)
                x->push_back(y->at(i));
            if (y->next != nullptr)
                y->next->prev = x;
            x->next = y->next;
        }

        block* find_block(size_type &index) const{
            block *ptr = head;
            while (ptr->size() <= index && ptr->next != nullptr) {
                index -= ptr->size();
                ptr = ptr->next;
            }
            return ptr;
        }
        
        _Tp* find(size_type index) const{
            return find_block(index)->data(index);
        }
        
        block *head;
        size_type count;
    };
}

namespace sjtu{
    using namespace sakura;
}

#endif

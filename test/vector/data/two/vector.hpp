#ifndef _vector_hpp
#define _vector_hpp

#include "algorithm.hpp"
#include "error.hpp"

namespace sakura{
#define INDEX_ERROR_HANDLER if (index < 0 || index >= count) throw("access denied in");
    template <typename _Tp>
    class vector{
        using size_type = unsigned long;
        
    private:
        _Tp *x;
        size_type count;
        size_type cap;
        
        void modify(size_type size) {
            cap = size;
            _Tp *t = new _Tp [cap];
            for (size_type i = 0; i < count; ++i)
                t[i] = x[i];
            delete [] x;
            x = t;
        }
        
        void expand() {
            modify(cap ? cap * 2: 1);
        }
        
    public:
        vector() : x(nullptr), count(0), cap(0) {}
        
        vector(size_type size, _Tp v = _Tp())
            : x(new _Tp [size]), count(size), cap(size) {
            for (size_type i = 0; i < count; ++i)
                x[i] = v;
        }
        
        vector(const vector& v)
            : x(new _Tp [v.cap]), count(v.count), cap(v.cap) {
            for (size_type i = 0; i < count; ++i)
                x[i] = v[i];
        }
        
        virtual ~vector() {
            if (x != nullptr)
                delete [] x;
        }

    public:
        const vector& operator= (const vector& v) {
            if (cap != 0) delete [] x;
            
            count = v.count; cap = v.cap;
            x = new _Tp [cap];
            for (size_type i = 0; i < count; ++i) 
                x[i] = v[i];

            return *this;
        }
        
    public:
        bool empty() const{
            return count == 0;
        }
        size_type size() const{
            return count;
        }
        size_type capacity() const{
            return cap;
        }
        
    public:
        void reserve(size_type size) {
            if (count + size > cap)
                modify(count + size);
        }
        void shrink_to_fit() {
            modify(count);
        }
        void swap(vector& v) {
            if (x != v.x) {
                swap(x, v.x);
                swap(count, v.count);
                swap(cap, v.cap);
            }
        }
        
    public:
        void clear() { count = 0; }
        void resize(size_type size) {
            modify(size);
            count = size;
        }
        void resize(size_type size, _Tp v) {
            count = cap = size;
            delete [] x;
            x = new _Tp [size];
            for (size_type i = 0; i < count; ++i)
                x[i] = v;
        }
        void push_back(const _Tp& v) {
            if (count == cap)
                expand();
            x[count++] = v;
        }
        void pop_back() {
            count--;
        }
        void insert(size_type index, _Tp v) {
            if (cap == count) expand();
            for (size_type i = count; i > index; --i)
                x[i] = x[i - 1];
            x[index] = v;
            ++count;
        }
        void erase(size_type index) {
            for (size_type i = index + 1; i < count; ++i)
                x[i - 1] = x[i];
            --count;
        }

        bool operator== (const vector& vp) const{
            if (count != vp.count) return false;
            for (size_type i = 0; i < count; ++i)
                if (x[i] != vp[i]) return false;
            return true;
        }
        
        bool operator< (const vector& vp) const{
            size_type len = min(count, vp.count);
            for (size_type i = 0; i < count; ++i)
                if (x[i] < vp[i]) return true;
            return count < vp.count;
        }

        bool operator> (const vector& vp) const{
            size_type len = min(count, vp.count);
            for (size_type i = 0; i < count; ++i)
                if (x[i] > vp[i]) return true;
            return count > vp.count;
        }

        bool operator!= (const vector& vp) const{ return !(*this == vp); }
        bool operator<= (const vector& vp) const{ return !(*this > vp); }
        bool operator>= (const vector& vp) const{ return !(*this < vp); }

    public:
        _Tp& front() { return x[0]; }
        _Tp& back() { return x[count - 1]; }
        _Tp& at(size_type index) {
            INDEX_ERROR_HANDLER
            return x[index];
        }
        _Tp& operator [] (size_type index) {
            INDEX_ERROR_HANDLER
            return x[index];
        }
        
        const _Tp& front() const{ return x[0]; }
        const _Tp& back() const{ return x[count - 1]; }
        const _Tp& at(size_type index) const{ return x[index]; }
        const _Tp& operator [] (size_type index) const{
            INDEX_ERROR_HANDLER
            return x[index];
        }
        
    public:
        class iterator{
            _Tp *ptr;

        public:
            iterator () = default;
            iterator (const iterator&) = default;
            iterator (_Tp *_ptr) : ptr(_ptr) {}

            size_type operator- (const iterator& vp) const{ return ptr - vp.ptr; }
            bool operator==  (const iterator& vp) const{ return ptr == vp.ptr; }
            bool operator!=  (const iterator& vp) const{ return ptr != vp.ptr; }

            bool operator<  (const iterator& vp) const{ return ptr < vp.ptr; }
            bool operator>  (const iterator& vp) const{ return ptr > vp.ptr; }
            bool operator<= (const iterator& vp) const{ return ptr <= vp.ptr; }
            bool operator>= (const iterator& vp) const{ return ptr >= vp.ptr; }
            
            iterator& operator++() { ptr++; return *this; }
            iterator& operator--() { ptr--; return *this; }
            iterator operator++(int) {
                iterator copy(*this);
                operator++();
                return copy;
            }
            iterator operator--(int) {
                iterator copy(*this);
                operator--();
                return copy;
            }

            iterator operator+  (size_type offset) const{ return ptr + offset; }
            iterator operator-  (size_type offset) const{ return ptr - offset; }
            iterator operator+= (size_type offset) { return *this = *this + offset; }
            iterator operator-= (size_type offset) { return *this = *this - offset; }
            
            _Tp& operator* () { return *ptr; }
            _Tp* operator->() { return  ptr; }
            
        }; // namespace vector::iterator

        class const_iterator{
            const _Tp *ptr;

        public:
            const_iterator () = default;
            const_iterator (const const_iterator&) = default;
            const_iterator (_Tp *_ptr) : ptr(_ptr) {}

            size_type operator- (const const_iterator& vp) const{ return ptr - vp.ptr; }
            bool operator==  (const const_iterator& vp) const{ return ptr == vp.ptr; }
            bool operator!=  (const const_iterator& vp) const{ return ptr != vp.ptr; }

            bool operator<  (const const_iterator& vp) const{ return ptr < vp.ptr; }
            bool operator>  (const const_iterator& vp) const{ return ptr > vp.ptr; }
            bool operator<= (const const_iterator& vp) const{ return ptr <= vp.ptr; }
            bool operator>= (const const_iterator& vp) const{ return ptr >= vp.ptr; }
            
            const_iterator& operator++() { ptr++; return *this; }
            const_iterator& operator--() { ptr--; return *this; }
            const_iterator operator++(int) {
                const_iterator copy(*this);
                operator++();
                return copy;
            }
            const_iterator operator--(int) {
                const_iterator copy(*this);
                operator--();
                return copy;
            }

            const_iterator operator+  (size_type offset) const{ return ptr + offset; }
            const_iterator operator-  (size_type offset) const{ return ptr - offset; }
            const_iterator operator+= (size_type offset) { return *this = *this + offset; }
            const_iterator operator-= (size_type offset) { return *this = *this - offset; }
            
            const _Tp& operator* () { return *ptr; }
            
        }; // namespace vector::const_iterator
        
        iterator begin() const{ return iterator(x); }
        iterator end() const{ return iterator(x + count); }
        const_iterator cbegin() const{ return const_iterator(x); }
        const_iterator cend() const{ return const_iterator(x + count); }

        void insert(const iterator &it, _Tp v) {
            insert(it - this->begin(), v);
        }
        void erase(const iterator &it) {
            erase(it - this->begin());
        }
        
        
        _Tp* data(size_type index = 0) { return x + index; }
        
    }; // class vector
    
} // namespace rem

namespace sjtu{
    using namespace sakura;
}
#endif

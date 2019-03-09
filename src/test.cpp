#include <cassert>
#include "deque.hpp"
#define N 1000000
int main() {
    sakura::deque<int> d;
    for (int i = 1; i <= N; ++i) {
        d.push_back(i);
        d.push_front(i * 100);
    }
    
    for (int i = N; i; --i) {
        //printf("%d %d\n", d.front(), d.back());
        assert(d.front() == i * 100);
        assert(d.back() == i);
        d.pop_front();
        d.pop_back();
    }
}

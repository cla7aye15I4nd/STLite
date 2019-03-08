#include <cassert>
#include "deque.hpp"

#define N 100000
int main() {
    sakura::deque<int> d;

    for (int i = 1; i <= N; ++i) {
        d.push_back(i);
        d.push_front(i * 10);
    }

    for (int i = N; i; --i) {
        //printf("%d %d\n", d.front(), d.back());
        assert(d.front() == i * 10);
        assert(d.back() == i);
        d.pop_front();
        d.pop_back();
    }
    
    return 0;
}

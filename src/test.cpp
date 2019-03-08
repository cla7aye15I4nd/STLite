#include <cassert>
#include <algorithm>
#include <iostream>
#define N 8
#include "map.hpp"

int a[N + 1];
int main() {
    sakura::map<int, int> map;

    for (int i = 1; i <= N; ++i)
        a[i] = i;
    std::random_shuffle(a + 1, a + N + 1);
    for (int i = 1; i <= N; i ++) {
        map.insert(a[i], a[i] * 2 + 1);
        map.print();puts("");
    }
    for (int i = 1; i <= N; ++i)
        assert(map.get(a[i]) == a[i] * 2 + 1);
    
    for (int i = 1; i <= N; ++i) {
        printf("#x%d\n", a[i]);
        map.print();
        puts("");
        map.erase(a[i]);
    }
    for (int i = 1; i <= N; ++i)
        map.insert(i, i);
    for (int i = 1; i <= N; ++i)
        assert(map.get(a[i]) == a[i]);
    
    return 0;
}

#include "priority_queue.hpp"
#define N 80000000
int main() {
    sakura::priority_queue<int> q;
    for (int i = 1; i <= N; ++i)
        q.push(i);
}

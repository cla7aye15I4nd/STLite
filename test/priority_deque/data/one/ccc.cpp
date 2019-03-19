#include <iostream>
#include <queue>
#include <ctime>
#include <cstdlib>

#include "../../../../src/priority_queue.hpp"

void TestConstructorAndPush()
{
	sjtu::priority_queue<int> pq;
	for (int i = 2; i; --i) {
		pq.push(i);
	}
	//sjtu::priority_queue<int> pqBack(pq);

}

int main(int argc, char *const argv[])
{
	TestConstructorAndPush();
	return 0;
}

#include <algorithm>
#include <map>
#include "map.hpp"
using namespace std;
int main() {
    sjtu::map<int, int> Q;
    std::map<int, int> stdQ;
    int num[3001];
    num[0] = 0;
    for(int i = 1; i <= 3000; i++) num[i] = num[i - 1] + rand() % 325 + 1; 
    for(int i = 1; i <= 6000; i++) swap(num[rand() % 3000 + 1], num[rand() % 3000 + 1]);
    for(int i = 1; i <= 3000; i++){
        int t = rand();
        stdQ[num[i]] = t; Q[num[i]] = t;
    }
    sjtu::map<int, int>::iterator it;
    std::map<int, int>::iterator stdit;
    for(int i = 1; i <= 6000; i++) swap(num[rand() % 3000 + 1], num[rand() % 3000 + 1]);
	
    for(int i = 1; i <= 1325; i++){
        it = Q.find(num[i]); 
        Q.erase(it);
        stdit = stdQ.find(num[i]); stdQ.erase(stdit);
    }

    auto iit = Q.find(481448); iit--;
    if (iit->first == 481488)
        printf("# %d\n", iit->first);
    
    stdit = --stdQ.end();
    for(it = --Q.end(); it != Q.begin(); it--){
        printf("%d %d\n", stdit -> first, it -> first);
        if(stdit -> first != it -> first) return 0;
        if(stdit -> second != (*it).second) return 0;
        stdit--;
    }
    return 1;
        
}

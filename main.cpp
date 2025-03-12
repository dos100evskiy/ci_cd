#include <iostream>
#include <unordered_map>

using namespace std;
unordered_map<int, long long> memo;
long long fib(int);

int main (int argc, char* argv[]) {
    if(argc != 2) {
        cerr << "ERROR USE ONLY ONE ARG" << endl;
        return 1;
    }

    if(stoi(argv[1]) > 92 || stoi(argv[1]) < 0) {
        cerr << "NOT USE ARG MORE THEN 92 OR LESS THEN 0" << endl;
        return 1;
    }

    cout << fib(stoi(argv[1])) << endl;
    return 0;
}

long long fib( int n ) {
    if( n == 0 || n == 1 )
        return n;
    if(memo.find(n) != memo.end()) 
        return memo[n];

    memo[n] = fib( n - 1 ) + fib( n - 2 );
    return memo[n];
}

#include <iostream>
#include <map>

using namespace std;
std::map<int, int> a = {{1, 1}};

int main() { std::cout << a[1] << " " << a[2] << std::endl; }

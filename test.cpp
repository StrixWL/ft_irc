#include <algorithm>
#include <string>
#include <iostream>
#include <vector>

int main() {
	std::vector<int> v;
	v.push_back(21);
	v.push_back(211);
	int k = *(v.data() + 1);
	std::cout << k << std::endl;
}

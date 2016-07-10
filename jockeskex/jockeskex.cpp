#include "stdafx.h"
#include <chrono>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <set>

int mex(std::vector<int>& lst);
int gv_next(const std::vector<int>& lst);
void gv_listing(int n, std::vector<int>& lst);

using namespace std::chrono;


int _tmain(int argc, _TCHAR* argv[])
{
	std::vector<int> lst = { 0, 1, 0, 2, 2, 4, 1, 1 };

	auto t1 = high_resolution_clock::now();
	gv_listing(5000, lst);
	auto t2 = high_resolution_clock::now();

	auto duration = duration_cast<seconds> (t2 - t1).count();
	std::cout << duration << std::endl;

	//std::for_each(lst.begin(), lst.end(), [](int i) {std::cout << i << ' '; });

	std::getchar();
}

int mex(std::vector<int>& lst) {
	std::sort(lst.begin(), lst.end());

	if (lst[0] != 0) return 0;
	int size = lst.size();
	for (int i = 0; i < size - 1; i++) {
		int diff = lst[i + 1] - lst[i];
		if (diff > 1)
			return lst[i]+1;
	}
	return *lst.rbegin() + 1;
}

int gv_next(const std::vector<int>& lst) {
	std::vector<int> value_list;
	int size = lst.size();

	for (int i = 0; i <= size / 2; i++)
		value_list.push_back(lst[i] ^ lst[lst.size() - i - 2]);
	for (int i = 1; i <= size / 2; i++)
		value_list.push_back(lst[i] ^ lst[lst.size() - i - 1]);
	return mex(value_list);

}

void gv_listing(int n, std::vector<int>& lst) {
	for (int i = 0; i < n; i++) {
		lst.push_back(gv_next(lst));
	}
}
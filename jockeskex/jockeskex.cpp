#include "stdafx.h"
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

int mex(const std::vector<int>& lst);
int gv_next(const std::vector<int>& lst);
void gv_listing(int n, std::vector<int>& lst);

int _tmain(int argc, _TCHAR* argv[])
{
	std::vector<int> lst = { 0, 1, 0, 2, 2, 4, 1, 1 };

	gv_listing(2000, lst);
	std::for_each(lst.begin(), lst.end(), [](int i) {std::cout << i << ' '; });

	std::getchar();
}

int mex(const std::vector<int>& lst) {
	//TODO: Sort first? Is it worth it?
	int i = 0;
	auto index = std::find(lst.begin(), lst.end(), i);
	while (index != lst.end()) {
		i++;
		index = std::find(lst.begin(), lst.end(), i);
	}
	return i;
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
#include "stdafx.h"
#include <chrono>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <set>

using namespace std;
using namespace chrono;

int mex(vector<int>& lst);
int gv_next(const vector<int>& lst);
void gv_listing(int n, vector<int>& lst);
int number_of_set_bits(int k);

int _tmain(int argc, _TCHAR* argv[])
{
	vector<int> lst = { 0, 1, 0, 2, 2, 4, 1, 1 };

	auto t1 = high_resolution_clock::now();
	//gv_listing(5000, lst);
	std::cout << number_of_set_bits(40);
	auto t2 = high_resolution_clock::now();

	auto duration = duration_cast<seconds> (t2 - t1).count();
	cout << endl << duration << endl;

	//for_each(lst.begin(), lst.end(), [](int i) {cout << i << ' '; });

	getchar();
}

int mex(vector<int>& lst) {
	sort(lst.begin(), lst.end());

	if (lst[0] != 0) return 0;
	int size = lst.size();
	for (int i = 0; i < size - 1; i++) {
		int diff = lst[i + 1] - lst[i];
		if (diff > 1)
			return lst[i]+1;
	}
	return *lst.rbegin() + 1;
}

int gv_next(const vector<int>& lst) {
	vector<int> value_list;
	int size = lst.size();

	for (int i = 0; i <= size / 2; i++)
		value_list.push_back(lst[i] ^ lst[lst.size() - i - 2]);
	for (int i = 1; i <= size / 2; i++)
		value_list.push_back(lst[i] ^ lst[lst.size() - i - 1]);
	return mex(value_list);

}

void gv_listing(int n, vector<int>& lst) {
	for (int i = 0; i < n; i++) {
		lst.push_back(gv_next(lst));
	}
}

vector<int> sp_list(int k) {
	vector<int> return_list;

	uint32_t binary_value = 4059; //Magic number

	for (uint32_t i = 0; i < k; i++){
		auto tmp = i & binary_value;
		if (number_of_set_bits(tmp) % 2 == 0)
			return_list.push_back(tmp);
	}
	return return_list;
}

//Stolen from http://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
//magic :0
int number_of_set_bits(int i) {
	i = (uint32_t)i;
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}
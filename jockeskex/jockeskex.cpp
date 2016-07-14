#include "stdafx.h"
#include <chrono>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <set>
#include <list>
using namespace std;

int mex(vector<int>& lst);
int gv_next(const vector<int>& lst);
void gv_listing(int n, vector<int>& lst);
int number_of_set_bits(int k);
vector<int> sp_list(int k);
int min_excl_common(vector<int> vl, vector<int> co);
int gvs_next(const vector<int> & lst, vector<int> co, vector<int> sp);
void gvs_listing(vector<int> & glst, vector<int> co, vector<int> sp, int n);

int _tmain(int argc, _TCHAR* argv[])
{
	vector<int> lst = { 0, 1, 0, 2, 2, 4, 1, 1 };

	auto t1 = chrono::high_resolution_clock::now();
	gv_listing(2000, lst);
	auto sparse = sp_list(100);
	
	//complement to sparse
	vector<int> common;
	for (int i = 0; i < 100; i++) {
		if (find(sparse.begin(), sparse.end(), i) == sparse.end()) {
			common.push_back(i);
		}
	}

	//Find indices of sparse values
	vector<int> sparse_indices;
	for (int i = 0; i < (1795 + 1); i++) {
		if (find(sparse.begin(), sparse.end(), lst[i]) != sparse.end())
			sparse_indices.push_back(i);
	}
	
	gvs_listing(lst, common, sparse_indices, 997992);//In total the first 8+7992+2000 = 10000 values

	

	auto t2 = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::seconds> (t2 - t1).count();
	cout << duration << endl;

	//vector<int> myvector = { 1, 2, 5, 6, 8, 11, 12, 15, 16};
	//cout << min_excl_common(myvector, common);
	
	//for (auto i = lst.begin(); i != lst.end(); ++i)
	//	std::cout << *i << ' ';

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
			return_list.push_back(i);
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

//vlst = value_list, co = common
int min_excl_common(vector<int> vlst, vector<int> co) {
	for (auto it = co.begin(); it != co.end(); ++it) {
		bool found =
			(find(vlst.begin(), vlst.end(), *it) != vlst.end());
		if (! found) return *it;
	}
}

//glst = list of grundy values, co = common values, sp = sparse_indices
int gvs_next(const vector<int> & glst, vector<int> co, vector<int> sp) {
	vector<int> value_list;
	int size = glst.size();
	for (auto it = sp.begin(); it != sp.end(); ++it) {
		value_list.push_back(glst[*it] ^ glst[size - *it - 2]);
	}
	for (auto it = sp.begin() + 1; it != sp.end(); ++it) {
		value_list.push_back(glst[*it] ^ glst[size - *it - 1]);
	}
	return min_excl_common(value_list, co);
}

//lst = list of grundy values, b = common values, c = sparse indices
void gvs_listing(vector<int> & glst, vector<int> co, vector<int> sp, int n) {
	for (int i = 0; i < n; i++) {
		glst.push_back(gvs_next(glst, co, sp));
//		cout << glst.back();
	}
}
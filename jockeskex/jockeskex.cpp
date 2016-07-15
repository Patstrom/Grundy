#include "stdafx.h"
#include <chrono>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <crtdbg.h>

#define _CRTDBG_MAP_ALLOC

#define GRUNDY_VALUES 100000000
#define SPARSE_VALUES 100
#define COMMON_VALUES 100
#define SPARSE_INDICES 1795


using namespace std;

int mex(const vector<int>& lst);
int gv_next();
void gv_listing(int n);
int number_of_set_bits(int k);
void sp_list();
int min_excl_common();
int gvs_next();
void gvs_listing(int n);
// Make these arrays? Is that faster?
static vector<int> grundy_values; 
static vector<int> sparse_values;
static vector<int> common_values;
static vector<int> sparse_indices;

static vector<int> gvs_next_value_lst;

void reserve() {
	grundy_values.reserve(GRUNDY_VALUES);
	sparse_values.reserve(SPARSE_VALUES);
	common_values.reserve(COMMON_VALUES);
	sparse_indices.reserve(SPARSE_INDICES);
	gvs_next_value_lst.reserve(SPARSE_INDICES * 2);
}

int _tmain(int argc, _TCHAR* argv[])
{
	reserve();
	grundy_values = { 0, 1, 0, 2, 2, 4, 1, 1 };

	auto t1 = chrono::high_resolution_clock::now();
	gv_listing(2000);
	sp_list();
	
	//complement to sparse
	for (int i = 0; i < COMMON_VALUES; ++i) {
		if (find(sparse_values.begin(), sparse_values.end(), i) == sparse_values.end()) {
			common_values.push_back(i);
		}
	}

	//Find indices of sparse_values values
	for (int i = 0; i < (SPARSE_INDICES + 1); ++i) {
		if (find(sparse_values.begin(), sparse_values.end(), grundy_values[i]) != sparse_values.end())
			sparse_indices.push_back(i);
	}
	
	gvs_listing(GRUNDY_VALUES - 2008); //In total the first 8+7992+2000 = 10000 values

	auto t2 = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::seconds> (t2 - t1).count();
	cout << duration << endl;

	//vector<int> myvector = { 1, 2, 5, 6, 8, 11, 12, 15, 16};
	//cout << min_excl_common(myvector, common);
	
	//for (auto i = lst.begin(); i != lst.end(); ++i)
	//	std::cout << *i << ' ';

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); //Look for memory leaks
}

int mex(const vector<int> & lst) {
	int i = 0;
	while (std::find(lst.begin(), lst.end(), i) != lst.end()) {
		++i;
	}
	return i;
}

int gv_next() {
	vector<int> value_list;
	int size = grundy_values.size();
	value_list.reserve(size + 1);

	for (int i = 0; i <= size / 2; ++i)
		value_list.push_back(grundy_values[i] ^ grundy_values[size - i - 2]);
	for (int i = 1; i <= size / 2; ++i)
		value_list.push_back(grundy_values[i] ^ grundy_values[size - i - 1]);
	return mex(value_list);
}

void gv_listing(int n) {
	for (int i = 0; i < n; ++i) {
		grundy_values.push_back(gv_next());
	}
}

void sp_list() {
	uint32_t binary_value = 4059; //Magic number

	for (uint32_t i = 0; i < SPARSE_VALUES; ++i){
		auto tmp = i & binary_value;
		if (number_of_set_bits(tmp) % 2 == 0)
			sparse_values.push_back(i);
	}
}

//Stolen from http://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
//magic :0
int number_of_set_bits(int i) {
	i = (uint32_t)i;
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

// Optimize here. Can probably re-use old values
int min_excl_common() {
	for (auto it = common_values.begin(); it != common_values.end(); ++it) {
		bool found =
			(find(gvs_next_value_lst.begin(), gvs_next_value_lst.end(), *it) != gvs_next_value_lst.end());
		if (!found) return *it;
	}
}

//Optimization is probably possible here. Can we re-use old values?
int gvs_next() {
	int size = grundy_values.size();

	// For use with static vlist
	for (int i = 0; i < sparse_indices.size(); ++i) {
		int index = sparse_indices[i];
		gvs_next_value_lst[i] = (grundy_values[index] ^ grundy_values[size - index - 2]);
	}
	for (int i = 1; i < sparse_indices.size(); ++i) {
		int index = sparse_indices[i];
		gvs_next_value_lst[i] = (grundy_values[index] ^ grundy_values[size - index - 1]);
	}
	/* If no static returnlist
	for (auto it = sparse_indices.begin(); it != sparse_indices.end(); ++it) {
		vlist.push_back(grundy_values[*it] ^ grundy_values[size - *it - 2]);
	}
	for (auto it = sparse_indices.begin() + 1; it != sparse_indices.end(); ++it) {
		vlist.push_back(grundy_values[*it] ^ grundy_values[size - *it - 1]);
	}
	*/
	return min_excl_common();
}

void gvs_listing(int n) {
	for (int i = 0; i < n; ++i) {
		grundy_values.push_back(gvs_next());
	}
}
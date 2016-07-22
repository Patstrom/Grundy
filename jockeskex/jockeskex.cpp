#include "stdafx.h"
#include <chrono>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <crtdbg.h>
#include "math.h"

#define GRUNDY_VALUES 500000000
#define SPARSE_VALUES 100
#define COMMON_VALUES 100
#define SPARSE_INDICES 1795


using namespace std;

// Functions for finding grundy-values.
int mex(const vector<int>& lst);
int gv_next();
void gv_listing(int n);
int number_of_set_bits(int k);
void sp_list();
int min_excl_common();
int gvs_next();
void gvs_listing(int n);

// Functions for determining periodicity.
int pcheck(vector<int> & glst);
int prepercheck(vector<int> & glst, int p);
void ultpercheck(vector<int> & glst, int p, int pre_p, int t);	//t is a parameter of a given octal game,
																//the lowest non-zero digit, i.e. For the 
																//octal game 0.30400... (this is usually written
																//as 0.304), t = 3.
void total_check(vector<int> & glst, int t);

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

	gvs_next_value_lst.resize(sparse_indices.size() * 2); 

	gvs_listing(GRUNDY_VALUES - 2008); 

	auto t2 = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::seconds> (t2 - t1).count();

	/*for (auto it = grundy_values.begin(); it != grundy_values.end(); ++it)
		printf("%d, ", *it);*/
	cout << "The time for computing the grundy values is "<< duration << " seconds.\n";
	
	auto t3 = chrono::high_resolution_clock::now();
	total_check(grundy_values, 3);
	auto t4 = chrono::high_resolution_clock::now();
	auto duration2 = chrono::duration_cast<chrono::seconds> (t4 - t3).count();

	cout << "The time for determining ultimately periodcity is " << duration2 << " seconds.";

	cin.get();

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
	auto size = grundy_values.size();

	// For use with static vlist
	for (int i = 0; i < sparse_indices.size(); ++i) {
		gvs_next_value_lst[i] = (grundy_values[sparse_indices[i]] ^ grundy_values[size - sparse_indices[i] - 2]);
	}
	for (int i = 1; i < sparse_indices.size(); ++i) {
		gvs_next_value_lst[sparse_indices.size() + i] = (grundy_values[sparse_indices[i]] ^ grundy_values[size - sparse_indices[i] - 1]);
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

//
// FUNCTIONS FOR DETERMINING PERIODICITY
//

//p = period, 
//pcand = candidate for period (if no possible period, pcand = -1),
//k = amount of times the sequence repeats.
int pcheck(vector<int> & glst) {
	int p = 1;
	int pcand = -1;
	int size = glst.size();

	//cout << "hej";
	while (p <= size / 2)
	{
		int k = 1;
		while (k*p <= size) {
			if (size - p < k*p) {
				pcand = p;
				p = p*k - p + 1;//DET MAGISKA HOPPET! ANVÄNDER SATS FÖR ATT BERÄTTIGA DETTA!
				break
					;
			}
			else {
				for (int i = 0; i < p; ++i) {
					if (glst[size - 1 - i] != glst[size - 1 - i - k*p]) {
						if (k > 1) {
							pcand = p;
							p = p*k - p + 1;//p*k-p+2???
							;
						}
						else {
							p++;
							;
						}
						k = size / p;
						break
							;
					}
					;
				}
				k++;
				;
			}
			;
		}
		;
	}
	return pcand;
};

int prepercheck(vector<int> & glst, int p) {
	int pre_p = -1;
	int size = glst.size();
	int quot = floor(size / p);

	for (int i = 1; i < quot; i++) {//Starting value for i? i = 1, 2? 
									//2 Should be OK (from speed POV) but maybe gets in trouble??
									//Shouldn't matter much.
		for (int j = 0; j < p; j++) {
			if (glst[size - 1 - j] != glst[size - 1 - j - i*p]) {
				pre_p = size - j - i*p;
				return pre_p;
				;
			}
		}
	}
	int extra = size - p*quot;
	for (int k = 0; k < extra; k++) {
		if (glst[size - 1 - k] != glst[size - 1 - k - quot*p]) {
			pre_p = size - k - quot*p;
			return pre_p;
		}
		;
	}
	pre_p = 0;
	return pre_p;
}

void ultpercheck(vector<int> & glst, int p, int pre_p, int t) {
	if (p == -1) {
		cout << "List not ultimately periodic/Not sufficiently long." << "\n";
		;
	}
	else {

		int size = glst.size();
		if (size >= 2 * p + 2 * pre_p + t) {
			cout << "The list is ultimately periodic with preperiod " << pre_p << " and period " << p << ".\n";
			;
		}
		else {
			cout << "List not ultimately periodic/Not sufficiently long."<< "\n";
			;
		}
		;
	}
	;
}

void total_check(vector<int> & glst, int t) {
	int period = pcheck(glst);
	int preperiod = prepercheck(glst, period);
	ultpercheck(glst, period, preperiod, t);
}
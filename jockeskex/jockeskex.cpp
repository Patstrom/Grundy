#include "stdafx.h"
#include <chrono>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <crtdbg.h>
#include "math.h"

#define GRUNDY_VALUES 10000000
#define GAME "4406"
#define BITSTRING "1111111111011"
#define SPARSE_VALUES 100
#define COMMON_VALUES 100
#define SPARSE_INDICES 500

using namespace std;

// Functions for finding grundy-values.
int mex(vector<int>lst);
void gv_listing(int stop);
int number_of_set_bits(int k);
void sp_list();
int min_excl_common(int lst);
void gvs_listing(int n);
vector <int> leave1Indices(string str); //Tested and OK!
vector <int> leave2Indices(string str); //Tested and OK!

// Functions for determining periodicity.
int pcheck(vector<int> & glst);
int prepercheck(vector<int> & glst, int p);
void ultpercheck(vector<int> & glst, int p, int pre_p, int t);	//t is a parameter of a given octal game,
																//the lowest non-zero digit, i.e. For the 
																//octal game 0.30400... (this is usually written
																//as 0.304), t = 3.
void total_check(vector<int> & glst);

// Make these arrays? Is that faster?
static vector<int> grundy_values; 
static vector<int> sparse_values;
static vector<int> common_values;
static vector<int> sparse_indices;

static vector<int> excludantVector;

void reserve() {
	grundy_values.reserve(GRUNDY_VALUES);
	sparse_values.reserve(SPARSE_VALUES);
	common_values.reserve(COMMON_VALUES);
	sparse_indices.reserve(SPARSE_INDICES);
}

int _tmain(int argc, _TCHAR* argv[])
{
	reserve();
	grundy_values = { 0 };
	
	auto t1 = chrono::high_resolution_clock::now();
	gv_listing(SPARSE_INDICES);

	sp_list();

	//complement to sparse
	for (int i = 0; i < COMMON_VALUES; ++i) {
		if (find(sparse_values.begin(), sparse_values.end(), i) == sparse_values.end()) {
			common_values.push_back(i);
		}
	}

	//Find indices of sparse_values values
	for (int i = 1; i < (SPARSE_INDICES + 1); ++i) {
		if (find(sparse_values.begin(), sparse_values.end(), grundy_values[i]) != sparse_values.end())
			sparse_indices.push_back(i);
	}

	excludantVector.resize(sparse_indices.size() * 2); 

	gvs_listing(GRUNDY_VALUES); 
	
	auto t2 = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::seconds> (t2 - t1).count();
	
	for (int i = 0; i < 20; ++i) {
		printf("%d, ", grundy_values[i]);
	}
	
	cout << "The time for computing "<< grundy_values.size()<<
		" grundy values is "<< duration << " seconds.\n";
	
	auto t3 = chrono::high_resolution_clock::now();
	total_check(grundy_values);
	auto t4 = chrono::high_resolution_clock::now();
	auto duration2 = chrono::duration_cast<chrono::seconds> (t4 - t3).count();

	cout << "The time for determining ultimately periodcity is " << duration2 << " seconds.";

	cin.get();
	
}

int mex(vector<int> lst) {
	int size = lst.size();
	if (size == 0) return 0;
	
	int largestVal = *max_element(lst.begin(), lst.end());
	
	if (largestVal == -1) {
		return 0;
	}
	vector<int> zeros(largestVal+1, 0);
	for (int i=0; i < size; ++i) {
		zeros[lst[i]] = 1;
	}
	for (int i=0; i <= largestVal; ++i) {
		if (zeros[i] == 0) {
			return i;
		}
	}
	return largestVal+1;
}

vector <int> leave0Indices(string str) {
	vector<int> leave0Vector = { 1, 3, 5, 7 };
	vector<int> strVector = {};

	int strLen = str.length();
	for (int i = 0; i < strLen; ++i) {
		int d_i = str[i] - '0';
		if (find(leave0Vector.begin(), leave0Vector.end(), d_i) != leave0Vector.end()) {
			strVector.push_back(i);
		}
	}

	return strVector;
}

vector <int> leave1Indices(string str) {
	vector<int> leave1Vector = { 2, 3, 6, 7 };
	vector<int> strVector = {};

	int strLen = str.length();
	for (int i = 0; i < strLen; ++i) {
		int d_i = str[i] - '0';
		if (find(leave1Vector.begin(), leave1Vector.end(), d_i) != leave1Vector.end()) {
			strVector.push_back(i);
		}
	}

	return strVector;
}

vector <int> leave2Indices(string str) {
	vector<int> leave2Vector = { 4, 5, 6, 7 };
	vector<int> strVector = {};

	int strLen = str.length();
	for (int i = 0; i < strLen; ++i) {

		int d_i = str[i] - '0';
		if (find(leave2Vector.begin(), leave2Vector.end(), d_i) != leave2Vector.end()) {
			strVector.push_back(i);
		}
	}

	return strVector;
}

void gv_listing(int stop) {
	int start = grundy_values.size();
	string gameStr = GAME;
	int k = gameStr.size();
	vector<int>::size_type sz;
	vector<int> excludantVec;
	sz = excludantVec.capacity();
	excludantVec.reserve(stop*k);//GÖR DETTA SKILLNAD?

	vector<int> leave0Moves = leave0Indices(gameStr);
	vector<int> leave1Moves = leave1Indices(gameStr);
	vector<int> leave2Moves = leave2Indices(gameStr);

	//Ways to leave 0 piles.
	for (int n = start; n <= stop; ++n) {
		excludantVec = {};
		for (vector<int>::iterator it = leave0Moves.begin(); it != leave0Moves.end(); ++it) {
			if (n == *it) {
				excludantVec.push_back(0);
			}
		}

		//Ways to leave 1 pile.
		for (vector<int>::iterator it = leave1Moves.begin(); it != leave1Moves.end(); ++it) {
			if (n >= *it + 1) {
				excludantVec.push_back(grundy_values[n - *it]);
			}
		}

		//Ways to leave 2 piles.
		for (vector<int>::iterator it = leave2Moves.begin(); it != leave2Moves.end(); ++it) {
			if (n >= *it + 2) {
				for (int i = 1; i <= (n - *it) / 2; ++i) {
					excludantVec.push_back(grundy_values[i] ^ grundy_values[n - *it - i]);
				}
			}
		}

		//The next value is the mex of 
		grundy_values.push_back(mex(excludantVec));
	}
}

void sp_list() {
	char sparseStr[] = BITSTRING;
	char * ptr;
	uint32_t binary_value = strtol(sparseStr, & ptr, 2);

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

int min_excl_common(vector<int> lst) {
	for (vector<int>::iterator it = common_values.begin(); it != common_values.end(); ++it) {
		if (!(find(lst.begin(), lst.end(), *it) != lst.end())) {
			return *it;
		}
	}
}

//We assume that the length of the list L of grundy-values is at least k+2.
void gvs_listing(int stop) {
	int start = grundy_values.size();
	string gameStr = GAME;
	int k = gameStr.size();
	vector<int> excludantVec;
	int nextVal;

	vector<int> leave1Moves = leave1Indices(gameStr); 
	vector<int> leave2Moves = leave2Indices(gameStr);
	//excludantVec.reserve(leave1Moves.size() + leave2Moves.size());//GÖR DETTA SKILLNAD?
	
	for (int n = start; n <= stop; ++n) {
		excludantVec = {};
		for (vector<int>::iterator it = leave1Moves.begin(); it != leave1Moves.end(); ++it) {
				excludantVec.push_back(grundy_values[n - *it]);
		}
		for (vector<int>::iterator it = leave2Moves.begin(); it != leave2Moves.end(); ++it) {
			for (vector<int>::iterator sp = sparse_indices.begin(); sp != sparse_indices.end(); ++sp) {
				excludantVec.push_back(grundy_values[*sp] ^ grundy_values[n - *it - *sp]);
			}
		}
		
		nextVal = min_excl_common(excludantVec);
		grundy_values.push_back(nextVal);
	}

}


// FUNCTION FOR DETERMINING PERIODICITY
//
void total_check(vector<int> & glst) {
	int n = glst.size();
	string gameStr = GAME;
	int k = gameStr.size();
	int bestPeriod = -1;
	int bestPrePeriod = -1;
	int longestSequence = 0;

	int p = 1;
	while (p <= n/2) {
		for (int i = 0; i < n - p; ++i) {
			if (glst[n - 1 - i] != glst[n - 1 - p - i]) {
				if (i > longestSequence) {
					bestPeriod = p;
					bestPrePeriod = n-p-i;
				}
				if (i > p) {
					p = i;
				}
				else {
					++p;
				}
				break;
			}
		}
	}
	if ((bestPeriod > -1) && (n>=2*bestPeriod+2*bestPrePeriod+k)) {
		cout << "The list is ultimately periodic with preperiod " << 
			bestPrePeriod << " and period " << bestPeriod << ".\n";
	}
	else {
		cout << "List not ultimately periodic/Not sufficiently long." << "\n";
	}
}
// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "iostream"
#include "string"
#include "algorithm"
#include "vector"
#include "chrono"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//DECLARATION OF FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

int max_k(string o_str);	//o_str must be on the form "a.bcd...", a=0, 4
							//for an finite amount of octal digits, at least
							//one of the digits must be non-zero.

int mex(vector <int>);

vector<int> next_value_listing(string o_str, int n, int k, int last);//n = The amount of values to compute,
															//starts at 0.
															//last = the index of last known sparse value
															//of the given octal game.

int next_value(vector<int> vec, string o_str, int k);

int BitCount(int u);//STOLEN.
					//Uses unsigned int in original, difference?

vector <int> Sparse_ints(string bstr, int n);
vector <int> Common_ints(string bstr, int n);//The complement to Sparse_ints.

int max_excl_common(vector<int> value_vec, vector<int> excl_vec);	//value_vec = list of values.
																	//excl_vec = common values of the game.

int grundy_candidate(vector<int> vec, vector <int> co_vec, 
	vector <int> spi_vec, string o_str, int k, int size);//Computes a list of all rare^common = common values 
														//of a particular position. Returns the lowest 
														//common value that is not in this list.

vector <int> sparse_indexes(vector <int> gv_vec, vector <int> sp_vec, int last);//The indexes of the sparse values
																				//in the nim-sequence.

vector <int> gv_sparse_listing(vector <int> gv_vec, vector <int> co_vec, 
	vector <int> spi_vec,string o_str, int k, int start, int stop);  //Computes the nim-sequence of a given octal game
																	//from start (start is presumed to be after the last
																	//value) to stop.



// Functions for determining periodicity.

int pcheck(vector<int> & glst);
int prepercheck(vector<int> & glst, int p);
void ultpercheck(vector<int> & glst, int p, int pre_p, int k);	//t is a parameter of a given octal game,
																//the lowest non-zero digit, i.e. For the 
																//octal game 0.30400... (this is usually written
																//as 0.304), t = 3.
void total_check(vector<int> & glst, int k);

////////////////////////////////////////////////////////////////////////////////
//MAIN
////////////////////////////////////////////////////////////////////////////////

int main()
{	

	auto t_start = chrono::high_resolution_clock::now();

	string ostr = "4.064";
	string bstr = "11111101011";
	int n = 275000;
	int last = 470;
	int lastsparse = 200;//SOME NUMBER THAT IS BIGGER THAN THE MAX GV


	ostr.erase(1, 1);
	int big_k = max_k(ostr);

	vector<int> gvals = next_value_listing(ostr, n, big_k, last);
	
	if (last + big_k + 1 < n) { //If the #values we want to compute is enough to surpass the last 
								//sparse value by a "sufficiently" big margin.
		vector <int> SP_LIST = Sparse_ints(bstr, lastsparse);
		vector <int> CO_LIST = Common_ints(bstr, lastsparse);
		vector <int> SPARSE_INDEX_LIST = sparse_indexes(gvals, SP_LIST, last);
		int SIZE = gvals.size();
		gvals = gv_sparse_listing(gvals, CO_LIST,
			SPARSE_INDEX_LIST, ostr, big_k, SIZE, n);
	};
	/*for (vector<int>::const_iterator i = gvals.begin(); i != gvals.end(); ++i)
		cout << *i << ' ';
	cout << "\n";*/

	auto t_gvals = chrono::high_resolution_clock::now();
	auto duration1 = chrono::duration_cast<chrono::seconds> (t_gvals - t_start).count();

	cout << "The time for computing the grundy values is " << duration1 << ".\n";

	total_check(gvals, big_k);

	auto t_period = chrono::high_resolution_clock::now();
	auto duration2 = chrono::duration_cast<chrono::seconds> (t_period - t_gvals).count();

	cout << "The time for determining periodicity is " << duration2 << ".";

	cin.get();
}

//////////////////////////////////////////////////////////////////////////////
//OTHER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

//Returns the biggest integer k of the digit-string 'd_0.d_1d_2...d_k'
//such that d_k != 0. E.g. max_k("0.473010") = 5.
int max_k(string o_str) {
	int biggest = -1;
	int len = o_str.length();

	for (int i = 0; i < len; i++) {
		int bigger = o_str[i]-'0';

		if (bigger != 0) {
			biggest = i;
		}
	}
	return biggest;
}

//MEX is not optimal in complexity. Could fix this later.
//Do with sort instead will yield an O(n)-algorithm.
int mex(vector <int> vec) {
		int i = 0;
		while (find(vec.begin(), vec.end(), i) != vec.end()) {
			++i;
		}
		return i;
	}


//Given a vector of grundy-values, the octal game string and the k 
//from max-k-function, this function returns the next value in the
//nim-sequence, i.e. the next value that should be in the vector.
int next_value(vector <int> vec, string o_str, int k) {
	int size = vec.size();
	vector<int> value_list;

	vector<int> vec0 = { 1, 3, 5, 7 };
	vector<int> vec1 = { 2, 3, 6, 7 };
	vector<int> vec2 = { 4, 5, 6, 7 };

	for (int i = 0; i <= k; i++) {
		int digit_i = o_str[i]-'0';

		if ((find(vec0.begin(), vec0.end(), digit_i) != vec0.end()) &&
			(size == i)) {
			value_list.push_back(0);
		}

		if ((find(vec1.begin(), vec1.end(), digit_i) != vec1.end()) &&
			(size > i)) {
			value_list.push_back(vec[size - i]);
		}
		
		if ((find(vec2.begin(), vec2.end(), digit_i) != vec2.end()) &&
			(size > i + 1)) {
			for (int j = 1; j <= ceil((size - i) / 2); j++) {
				value_list.push_back(vec[j] ^ vec[size - i - j]);
			}
		}
	}
	return mex(value_list);
	;
}


//Returns the nim-sequence of length min(n, last)+1 of a given octal game.
//This algorithm is used to compute values up untill the last known sparse
//value, after that sparse space algorithm is faster.
vector<int> next_value_listing(string o_str, int n, int k, int last) {
	int size = min(last+1+k, n);//+1 to prevent index-problems!
	vector <int> gv_list = { 0 };//0 always first value by def.

	for (int i = 1; i <= size; i++) {
		int next = next_value(gv_list, o_str, k);
		gv_list.push_back(next);
	}
	return gv_list;
}

//STOLEN from http://stackoverflow.com/questions/8871204/count-number-of-1s-in-binary-representation
/*int BitCount(unsigned int u)
{
	unsigned int uCount;

	uCount = u - ((u >> 1) & 033333333333) - ((u >> 2) & 011111111111);
	return ((uCount + (uCount >> 3)) & 030707070707) % 63;
}*/

//Given a non-negative integer n, this returns the number of ones in the binary representation of n.
int BitCount(int n) {//STOLEN from http://stackoverflow.com/questions/8871204/count-number-of-1s-in-binary-representation
	int count = 0;
	while (n != 0) {
		n = n&(n - 1);
		count++;
	}
	return count;
}

//Given an integer n, and a bitstring of a binary number b (e.g. "1110101") this returns a vector
//which contains all integers i, 0<=i<=n such that i&b has an even number ones in its
//binary representation.
vector <int> Sparse_ints(string bstr, int n) {
	int dec = stoull(bstr, 0, 2);
	vector <int> sp_list = { 0 };

	for (int i = 1; i <= n; i++) {
		int ones = BitCount(i&dec);
		if ((ones % 2) == 0) {
			sp_list.push_back(i);
		}
	}
	return sp_list;
}

vector <int> Common_ints(string bstr, int n) {
	int dec = stoull(bstr, 0, 2);
	vector <int> co_list;

	for (int i = 1; i <= n; i++) {
		int ones = BitCount(i&dec);
		if ((ones % 2) == 1) {
			co_list.push_back(i);
		}
	}
	return co_list;
}

//Returns the smallest value in excl_vec that is not in value_vec.
int max_excl_common(vector<int> value_vec, vector<int> excl_vec) {

	for (vector<int>::iterator it = excl_vec.begin(); it != excl_vec.end(); ++it) {
		if (! (find(value_vec.begin(), value_vec.end(), *it) != value_vec.end())) {
			return *it;
		}

	}
}

//Given a (nim-)sequence gv_vec, a list of sparse values and the index for the last sparse value
//of for a given octal game, return the indexes of all sparse values of the sequnce.
vector <int> sparse_indexes(vector <int> gv_vec, vector <int> sp_vec, int last) {
	vector <int> sp_indexes;// = { 0 };//first value is always 0 which is rare.

	for (int i = 1; i <= last; i++) {
		if (find(sp_vec.begin(), sp_vec.end(), gv_vec[i]) != sp_vec.end()) {
			sp_indexes.push_back(i);
		}
	}
	return sp_indexes;
}

//Given a nim-sequence (longer than the last known sparse value), return the option which has
//the lowest common value.
int grundy_candidate(vector <int> gv_vec, vector <int> co_vec,
	vector <int> spi_vec, string o_str, int k, int size) {

	int cand;
	vector<int> value_vec;
	vector<int> vec1 = { 2, 3, 6, 7 };//DON'T NEED TO CREATE NEW ONES.
	vector<int> vec2 = { 4, 5, 6, 7 };//LET THEM BE CONSTANT SOMEWHERE???

	for (int i = 0; i <= k; i++) {
		int digit_i = o_str[i] - '0';

		if (find(vec1.begin(), vec1.end(), digit_i) != vec1.end()) {
			value_vec.push_back(gv_vec[size - i]);
		}

		if (find(vec2.begin(), vec2.end(), digit_i) != vec2.end()) {
			for (vector<int>::iterator it = spi_vec.begin(); it != spi_vec.end(); ++it) {
				value_vec.push_back(gv_vec[*it] ^ gv_vec[size - i - *it]);
			}
		}
	}
	return max_excl_common(value_vec, co_vec);
}

//Returns a list of grundy-values using the grundy-candidate function.
vector <int> gv_sparse_listing(vector <int> gv_vec, vector <int> co_vec, 
	vector <int> spi_vec, string o_str, int k, int start, int stop) {

	for (int i = start; i <= stop; i++) {//start is the first "new" value to be computed, so start = gv.vec.size()
		int next = grundy_candidate(gv_vec, co_vec, spi_vec, o_str, k, i);
		gv_vec.push_back(next);
	}
	return gv_vec;
}//
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

void ultpercheck(vector<int> & glst, int p, int pre_p, int k) {
	if (p == -1) {
		cout << "List not ultimately periodic/Not sufficiently long." << "\n";
		;
	}
	else {

		int size = glst.size();
		if (size >= 2 * p + 2 * pre_p + k) {
			cout << "The list is ultimately periodic with preperiod " << pre_p << " and period " << p << ".\n";
			;
		}
		else {
			cout << "List not ultimately periodic/Not sufficiently long." << "\n";
			;
		}
		;
	}
	;
}

void total_check(vector<int> & glst, int k) {
	int period = pcheck(glst);
	int preperiod = prepercheck(glst, period);
	ultpercheck(glst, period, preperiod, k);
}
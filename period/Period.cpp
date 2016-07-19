// ConsoleApplication11.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "vector"
#include "iostream"
#include "math.h"


using namespace std;


int pcheck(vector<int> & glst);
int prepercheck(vector<int> & glst, int p);
void ultpercheck(vector<int> & glst, int p, int pre_p, int t);	//t is a parameter of a given octal game,
																//the lowest non-zero digit, i.e. For the 
																//octal game 0.30400... (this is usually written
																//as 0.304), t = 3.
void total_check(vector<int> & glst, int t);

int main()
{
	vector<int>gv_list = { 4, 5, 3, 4, 1, 3, 1, 3, 4, 1, 3, 1, 3, 4 };//Some "arbitrary" list.
	total_check(gv_list, 0);
	cin.get();
}

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
		cout << "List not ultimately periodic/Not sufficiently long.";
		;
	}
	else {

		int size = glst.size();
		if (size >= 2 * p + 2 * pre_p + t) {
			cout << "The list is ultimately periodic with preperiod " << pre_p << " and period " << p;
			;
		}
		else {
			cout << "List not ultimately periodic/Not sufficiently long.";
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
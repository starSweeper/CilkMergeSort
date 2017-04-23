/*
	Project to demonstrate merge sort using Cilk
	Resources:
		recursiveMergeSort heavily based off Merge-Sort pseudocode from book (page 34)
		parallelMergeSort heavily based off Merge-Sort pseudocode from book (page 797)
		merge heavily based off merge pseudocode from book (page 31)
*/

#include<iostream>
using namespace std;

void recursiveMergeSort(int[], int, int);
void parallelMergeSort(int[], int, int);
void merge(int[], int, int, int);

const int TOINFINITYANDBEYOND = 9999; //"Infinity" for our purposes.

int main() {

}

void recursiveMergeSort(int array[], int p, int r) {
	if (p < r) {
		int q = (p + r) / 2;
		recursiveMergeSort(array, p, q);
		recursiveMergeSort(array, q + 1, r);
		merge(array, p, q, r);
	}
}

void parallelMergeSort(int array[], int p, int r) {
	if (p < r) {
		int q = (p + r) / 2;
		//SPAWN mergeSort(array, p, q);
		parallelMergeSort(array, (q + 1), r);
		//SYNC
		merge(array, p, q, r);
	}
}

void merge(int array[], int p, int q, int r) {
	int n1 = q - p + 1;
	int n2 = r - q;
	int lArray[100];
	int rArray[100];
	int i;
	int j;

	for (i = 1; i < n1; i++) {
		lArray[i] = array[p + i - 1];
	}
	lArray[n1 + 1] = TOINFINITYANDBEYOND;
	i = 1;

	for (j = 1; j < n2; j++) {
		rArray[j] = array[q + 1];
	}
	rArray[n2 + 1] = TOINFINITYANDBEYOND;
	j = 1;

	for (int k = p; p < r; k++) {
		if (lArray[i] < rArray[j]) {
			array[k] = lArray[i];
			i++;
		}
		else{
			array[k] == rArray[j];
			j++;
		}
	}
}


/*
Code written by Thomas Carter, Tyler Hemphil and Amanda Panell

Command line paramaters:
	1- input file
	2- output file path 1 (recursive)
	3- output file path 2 (parallel merge sort w/ regular merge)
	4- output file path 3 (parallel merge sort w/ parallel mege)

Project to demonstrate merge sort using Cilk
Resources:
recursiveMergeSort heavily based off MERGE-SORT pseudocode from book (page 34)
parallelMergeSort heavily based off MERGE-SORT pseudocode from book (page 797)
pMergeSort heavily based off P-MERGE-SORT pseudocode from book (page 803)
merge heavily based off MERGE pseudocode from book (page 31)
pMerge heavily based off P-MERGE pseudocode from book (page 800)
binarySearch heavily based off BINARY-SEARCH pseudocode from book (page 799)
Cilk commands from https://www.cilkplus.org/tutorial-cilk-plus-keywords#spawn_and_sync
*/

#include<iostream>
#include<math.h>
#include<algorithm>
#include<fstream>

using namespace std;

void printArrayToFile(int[], string, int);
int processFile(string, int[]);
void recursiveMergeSort(int[], int, int);
void parallelMergeSort(int[], int, int);
void pMergeSort(int[], int, int, int[], int);
void merge(int[], int, int, int);
void pMerge(int[], int, int, int, int, int[], int);
int binarySearch(int, int[], int, int);

const int TOINFINITYANDBEYOND = 999999; //"Infinity" for our purposes.

int main(int argc, char *argv[]) {
	int bArray[TOINFINITYANDBEYOND];
	int intArray[TOINFINITYANDBEYOND];
	int count = processFile(argv[1], intArray); //Open file and get count
	
	int arrayToSortRecursive[TOINFINITYANDBEYOND];
	int arrayToSortParallel[TOINFINITYANDBEYOND];
	int arrayToSortParallelMerge[TOINFINITYANDBEYOND];

	recursiveMergeSort(arrayToSortRecursive, 1, count);
	printArrayToFile(arrayToSortRecursive, argv[2], count);
	parallelMergeSort(arrayToSortParallel, 1, count);
	printArrayToFile(arrayToSortParallel, argv[3], count);
	pMergeSort(arrayToSortParallelMerge, 1, count, bArray, 0);
	printArrayToFile(arrayToSortParallelMerge, argv[4], count);
}

void printArrayToFile(int arrayToPrint[], string filePath, int arrayLength) {
	ofstream outFile;
	outFile.open(filePath);

	for (int i = 0; i < arrayLength; i++) {
		outFile << arrayToPrint[i];
	}

	outFile.close();
}

int processFile(string filePath, int intArray[]) {
	ifstream inputFile;
	int count = 0;

	inputFile.open(filePath);

	while (inputFile >> intArray[count]) {
		count++;
	}

	inputFile.close();

	return count;
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
		cilk_spawn mergeSort(array, p, q);
		parallelMergeSort(array, (q + 1), r);
		cilk_sync;
		merge(array, p, q, r);
	}
}

void pMergeSort(int Aarray[], int p, int r, int Barray[], int s)
{
	int n = r - p + 1;
	if (n == 1)
	{
		Barray[s] = Aarray[p];
	}
	else {
		int Tarray[TOINFINITYANDBEYOND];
		int q = floor((p + r) / 2);
		int qprime = q - p + 1;

		cilk_spawn pMergeSort(Aarray, p, q, Tarray, 1);
		pMergeSort(Aarray, (q + 1), r, Tarray, (qprime + 1));
		cilk_sync;
		pMerge(Tarray, 1, qprime, (qprime + 1), n, Barray, s);
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
		else {
			array[k] == rArray[j];
			j++;
		}
	}
}

void pMerge(int Tarray[], int p1, int r1, int p2, int r2, int Aarray[], int p3) {
	int n1 = r1 - p1 + 1;
	int n2 = r2 - p2 + 1;

	if (n1 < n2)
	{
		int tempp1 = p1;
		p1 = p2;
		p2 = tempp1;

		int tempr1 = r1;
		r1 = r2;
		r2 = tempr1;

		int tempn1 = n1;
		n1 = n2;
		n2 = tempn1;
	}

	if (n1 == 0)
	{
		return;
	}

	else {
		int q1 = floor((p1 + r1) / 2);
		int q2 = binarySearch(Tarray[q1], Tarray, p2, r2);
		int q3 = p3 + (q1 - p1) + (q2 - p2);
		Aarray[q3] = Tarray[q1];
		cilk_spawn pMerge(Tarray, p1, (q1 - 1), p2, (q2 - 1), Aarray, p3);
		pMerge(Tarray, (q1 + 1), r1, q2, r2, Aarray, (q3 + 1));
		cilk_sync;
	}
}

int binarySearch(int x, int Tarray[], int p, int r) {
	int low = p;
	int high = max(p, r + 1);

	cilk_for(; low < high; ) {
		mid = floor((low + high) / 2);
		if (x <= Tarray[mid]) {
			high = mid;
		}
		else {
			low = mid + 1;
		}
	}
	return high;
}


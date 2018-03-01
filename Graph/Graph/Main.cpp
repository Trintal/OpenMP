#define _CRT_SECURE_NO_WARNINGS
/*
LAB #3: MSD RADIX SORT FOR DOUBLE NUMBERS
NON PARALLEL AND PARALLEL VERSIONS
DESIGNED BY NIKOLAY KOMAROV
*/

#include "iostream"
#include <string>
#include <ctime>

#include <queue>

using namespace std;

int procSize;
int procRank;

enum sortingType { INCREASE };

sortingType orderOfSorting;

union BinaryInt
{
	int d;
	unsigned char c[sizeof(double)];

	BinaryInt()
	{
		d = 0;
	}
	BinaryInt(int doub)
	{
		d = doub;
	}
};

void welcomeWords(int *size)
{
	cout << "\n \nLAB #3: MSD RADIX SORT FOR DOUBLE NUMBERS\n \n";
	orderOfSorting = INCREASE;
	cout << "Enter size: ";
	cin >> *size;
}

void initData(BinaryInt **dataArray, int *size)
{
	*dataArray = new BinaryInt[*size];
	srand(time(NULL));
	rand();
	for (int i = 0; i < *size; i++)
	{
		(*dataArray)[i] = BinaryInt(rand());
	}
}

void copyData(BinaryInt **src, BinaryInt **dest, int *size)
{
	*dest = new BinaryInt[*size];
	for (int i = 0; i < *size; i++)
		(*dest)[i] = BinaryInt((*src)[i].d);
}

void printArray(BinaryInt **array, int *size)
{
	for (int i = 0; i < *size - 1; i++)
	{
		cout << (*array)[i].d << ", ";
	}
	cout << (*array)[*size - 1].d << "\n";
}

void outputMessage(double *time1, double *time2, bool isRight)
{
	cout << "\n--- RESULTS ---\n\nTime of non-parallel algorythm: " << *time1 <<
		" ms\nTime of parallel algorythm:     " << *time2 << " ms\n" <<
		"Speedup: " << *time1 / *time2 << "\n" <<
		"Results are the same: " << isRight << "\n";
}

void RadixSort(queue<BinaryInt> *data, queue<BinaryInt> *sortedData, int *numOfByte, int *numOfBitInByte)
{
	queue<BinaryInt> queueZero;
	queue<BinaryInt> queueOne;

	if (*numOfByte != -1)
	{
		while ((*data).size() != 0)
		{
			if (!(*numOfBitInByte & (*data).front().c[*numOfByte]))
			{
				queueZero.push((*data).front());
				(*data).pop();
			}
			else
			{
				queueOne.push((*data).front());
				(*data).pop();
			}
		}
		// recursive call must be outside of while loop above
		*numOfByte = *numOfBitInByte == 1 ? --*numOfByte : *numOfByte;
		*numOfBitInByte = *numOfBitInByte == 1 ? *numOfBitInByte = 64 : *numOfBitInByte = *numOfBitInByte / 2;
		int numOfByteCopy2 = *numOfByte;
		int numOfBitInByteCopy2 = *numOfBitInByte;

		if (queueZero.size() > 1)
			RadixSort(&queueZero, sortedData, numOfByte, numOfBitInByte);
		while (queueZero.size() != 0)
		{
			(*sortedData).push(queueZero.front());
			queueZero.pop();
		}

		if (queueOne.size() > 1)
			RadixSort(&queueOne, sortedData, &numOfByteCopy2, &numOfBitInByteCopy2);
		while (queueOne.size() != 0)
		{
			(*sortedData).push(queueOne.front());
			queueOne.pop();
		}
	}
}

BinaryInt* merge(BinaryInt **firstArray, BinaryInt **secondArray, int *sizeFirst, int *sizeSecond)
{
	BinaryInt *sortedArray = new BinaryInt[*sizeFirst + *sizeSecond];
	int indexFirst = 0;
	int indexSecond = 0;
	int index = 0;
	while ((indexFirst < *sizeFirst) && (indexSecond < *sizeSecond))
	{
		BinaryInt elementFirst = (*firstArray)[indexFirst];
		BinaryInt elementSecond = (*secondArray)[indexSecond];
		if (elementFirst.d < elementSecond.d)
		{
			sortedArray[index] = elementFirst;
			indexFirst++;
		}
		else
		{
			sortedArray[index] = elementSecond;
			indexSecond++;
		}
		index++;
	}

	while (indexFirst < *sizeFirst)
	{
		sortedArray[index] = (*firstArray)[indexFirst];
		indexFirst++;
		index++;
	}

	while (indexSecond < *sizeSecond)
	{
		sortedArray[index] = (*secondArray)[indexSecond];
		indexSecond++;
		index++;
	}
	return sortedArray;
}

void setResult(queue<BinaryInt> *sortedData, BinaryInt **data)
{
	int count = (*sortedData).size();
	for (int i = 0; i < count; i++)
	{
		(*data)[i] = (*sortedData).front();
		(*sortedData).pop();
	}
}

bool checkResult(BinaryInt **nonParallel, BinaryInt **parallel, int *size)
{
	for (int i = 0; i < *size; i++)
	{
		if ((*nonParallel)[i].d != (*parallel)[i].d)
			return false;
	}
	return true;
}

int main()
{
	cout.precision(15);
	int size;
	BinaryInt *nonParallel = nullptr;
	BinaryInt *parallel = nullptr;
	BinaryInt *parallelCopy = nullptr;
	int *sendCounts = nullptr;
	int *displs = nullptr;
	double startTime = 0;
	double endTime = 0;
	double timeOfNonParallel = 0;
	double timeOfParallel = 0;
	welcomeWords(&size);
	initData(&nonParallel, &size);
	cout << "[TRACE] Data was initialized \n";
	copyData(&nonParallel, &parallel, &size);
	cout << "[TRACE] Data was copied \n";
	cout << "[INFO ] Size equals: " << size << "\n";

	printArray(&nonParallel, &size);

	/* ---------- START OF NON PARALLEL ALGORITHM ------------------ */
	startTime = clock();
	cout << "[TRACE] Start time of non-parallel algorythm: " << startTime << "\n";

	queue<BinaryInt> queueData;
	queue<BinaryInt> sortedData;
	for (int i = 0; i < size; i++) {
		queueData.push(nonParallel[i]);
	}
	int u = 3; int o = 64;
	RadixSort(&queueData, &sortedData, &u, &o);
	cout << "[TRACE] Array was sorted by non-parallel algorythm \n";
	setResult(&sortedData, &nonParallel);

	endTime = clock();
	cout << "[TRACE] End time of non-parallel algorythm: " << endTime << "\n";
	timeOfNonParallel = endTime - startTime;
	cout << "[TRACE] Total time of non-parallel algorythm: " << timeOfNonParallel << " ms\n";

	printArray(&nonParallel, &size);

	system("pause");
	return 0;
}
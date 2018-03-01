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

union BinaryDouble
{
	double d;
	unsigned char c[sizeof(double)];

	BinaryDouble()
	{
		d = 0;
	}
	BinaryDouble(double doub)
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

void initData(BinaryDouble **dataArray, int *size)
{
	*dataArray = new BinaryDouble[*size];
	srand(time(NULL));
	rand();
	for (int i = 0; i < *size; i++)
	{
		(*dataArray)[i] = BinaryDouble((static_cast<double>(rand()) / RAND_MAX) * rand());
	}
}

void copyData(BinaryDouble **src, BinaryDouble **dest, int *size)
{
	*dest = new BinaryDouble[*size];
	for (int i = 0; i < *size; i++)
		(*dest)[i] = BinaryDouble((*src)[i].d);
}

void printArray(BinaryDouble **array, int *size)
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

void RadixSort(queue<BinaryDouble> *data, queue<BinaryDouble> *sortedData, int *numOfByte, int *numOfBitInByte)
{
	queue<BinaryDouble> queueZero;
	queue<BinaryDouble> queueOne;

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
		*numOfBitInByte = *numOfBitInByte == 1 ? *numOfBitInByte = 128 : *numOfBitInByte = *numOfBitInByte / 2;
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

BinaryDouble* merge(BinaryDouble **firstArray, BinaryDouble **secondArray, int *sizeFirst, int *sizeSecond)
{
	BinaryDouble *sortedArray = new BinaryDouble[*sizeFirst + *sizeSecond];
	int indexFirst = 0;
	int indexSecond = 0;
	int index = 0;
	while ((indexFirst < *sizeFirst) && (indexSecond < *sizeSecond))
	{
		BinaryDouble elementFirst = (*firstArray)[indexFirst];
		BinaryDouble elementSecond = (*secondArray)[indexSecond];
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

void setResult(queue<BinaryDouble> *sortedData, BinaryDouble **data)
{
	int count = (*sortedData).size();
	for (int i = 0; i < count; i++)
	{
		(*data)[i] = (*sortedData).front();
		(*sortedData).pop();
	}
}

bool checkResult(BinaryDouble **nonParallel, BinaryDouble **parallel, int *size)
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
	BinaryDouble *nonParallel = nullptr;
	BinaryDouble *parallel = nullptr;
	BinaryDouble *parallelCopy = nullptr;
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

	queue<BinaryDouble> queueData;
	queue<BinaryDouble> sortedData;
	for (int i = 0; i < size; i++) {
		queueData.push(nonParallel[i]);
	}
	int u = 7; int o = 128;
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
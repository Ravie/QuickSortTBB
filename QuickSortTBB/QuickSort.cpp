#include <iostream>
#include <ctime>
#include <algorithm>
#include <functional>
#include "tbb/tick_count.h"
//#include "tbb/parallel_invoke.h"
#include "tbb/task_group.h"
/*
using namespace tbb;
template<typename I>
void fork_join_qsort(I begin, I end) 
{
	typedef typename std::iterator_traits<I>::value_type T;
	if (begin != end) 
	{
		const I pivot = end - 1;
		const I middle = std::partition(begin, pivot, std::bind(std::less<T>(), std::placeholders::_1, *pivot));
		std::swap(*pivot, *middle);
		tbb::parallel_invoke(fork_join_qsort(begin, middle), fork_join_qsort(middle + 1, end));
	}
}
*/

void copy_array(int* source, int *destination, const long length)
{
	for (int n = 0; n < length; n++)
		destination[n] = source[n];
}

void quickSort(int* a, const long n)
{
	long i = 0, j = n;
	int pivot = a[n / 2]; // выбор опорного элемента
	do {
		while (a[i] < pivot) i++;
		while (a[j] > pivot) j--;
		if (i <= j)
		{
			std::swap(a[i], a[j]);
			i++; j--;
		}
	} while (i <= j);
	if (j > 0)
		quickSort(a, j);
	if (n > i)
		quickSort(a + i, n - i);
}

void par_quickSort(int* a, const long n)
{
	long i = 0, j = n;
	int pivot = a[n / 2]; // опорный элемент
	do {
		while (a[i] < pivot) i++;
		while (a[j] > pivot) j--;
		if (i <= j)
		{
			std::swap(a[i], a[j]);
			i++; j--;
		}
	} while (i <= j);
	if (n < 100)
	{ // если размер массива меньше 100
	  // сортировка выполн¤етс¤ в текущем потоке
		if (j > 0)
			par_quickSort(a, j);
		if (n > i)
			par_quickSort(a + i, n - i);
		return;
	}
	tbb::task_group g;
	g.run([&] {if (j > 0) par_quickSort(a, j); });
	g.run([&] {if (n > i) par_quickSort(a + i, n - i); });
	g.wait();
}

int check_sort(int* a, const long length)
{
	for (int n = 0; n < length - 1; n++)
		if (a[n]>a[n + 1])
			return 0;
	return 1;
}

int main()
{
	int MAX_NUMBER, MAX_SIZE;
	std::cout << "Size of array: ";
	std::cin >> MAX_SIZE;
	std::cout << "Max element in array: ";
	std::cin >> MAX_NUMBER;

	srand((int)time(0));
	int *Array = new int[MAX_SIZE];
	for (int n = 0; n < MAX_SIZE; n++)
		Array[n] = rand() % MAX_NUMBER;
	
	int *copy_of_array = new int[MAX_SIZE];
	copy_array(Array, copy_of_array, MAX_SIZE);

	tbb::tick_count start_par = tbb::tick_count::now();
	par_quickSort(Array, MAX_SIZE - 1);
	tbb::tick_count end_par = tbb::tick_count::now();
	std::cout << "Parallel sorting: " << (end_par - start_par).seconds() << std::endl;

	tbb::tick_count start_seq = tbb::tick_count::now();
	quickSort(copy_of_array, MAX_SIZE - 1);
	tbb::tick_count end_seq = tbb::tick_count::now();
	std::cout << "Sequence sorting: " << (end_seq - start_seq).seconds() << std::endl;

	std::cout << "Acceleration x" << ((end_seq - start_seq).seconds() / (end_par - start_par).seconds()) << std::endl;

	if (check_sort(Array, MAX_SIZE - 1))
		std::cout << "Array is sorted" << std::endl;
	else
		std::cout << "Array is unsorted" << std::endl;

	delete[] Array;
	delete[] copy_of_array;

	return 0;
}

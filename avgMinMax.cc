/*
Owen, Zachary
OS Concepts 351
Professor McCarthy

Question 4.21
*/

#include <iostream>
#include <thread>
using namespace std;

void average(int list[], int size);
void minimum(int list[], int size);
void maximum(int list[], int size);

double avg;
int min, max;

int main(int argc, char *argv[]) {
	int* arr = new int[argc - 1];
	char c;
	for (int i = 1; i < argc; i++) {
		c = *argv[i];
		arr[i - 1] = static_cast<int>(c) - '0';
	}


	thread threadOne (average, arr, argc - 1);
	thread threadTwo(minimum, arr, argc - 1);
	thread threadThree(maximum, arr, argc - 1);

	threadOne.join();
	threadTwo.join();
	threadThree.join();

	cout << avg << endl;
	cout << min << endl;
	cout << max << endl;

	system("Pause");
	return 0;
}

void average(int list[], int size) {
	int sum = 0;
	for (int i = 0; i < size; i++) {
		sum += list[i];
	}

	avg = static_cast<double>(sum) / size;
}

void minimum(int list[], int size) {

	if (size > 0) {
		min = list[0];
		for (int i = 1; i < size; i++) {
			if (list[i] < min) {
				min = list[i];
			}
		}
	}
	else cerr << "List is empty." << endl;
}

void maximum(int list[], int size) {
	if (size > 0) {
		max = list[0];
		for (int i = 1; i < size; i++) {
			if (list[i] > max) {
				max = list[i];
			}
		}
	}
	else cerr << "List is empty." << endl;
}
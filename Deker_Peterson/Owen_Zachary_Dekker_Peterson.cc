/*
	Owen, Zachary
	OS Concepts 351
	Professor McCarthy

	Peterson's Algorithm
	Dekker's Algorithm
*/
#include <iostream>
#include <thread>

using namespace std;

bool flag[2];
int turn = 0;
bool finishLine = true;
int sharedResource = 0;
#define TOTAL_TASKS_PZERO 20
#define TOTAL_TASKS_PONE 20
#define NUMBER_OF_RACES_PER_ALGORITHM 15;

void Peterson(int currentID) {
	int otherID = (currentID) ? 0 : 1; // Get the ID of the other thread.
	int taskNumber = 0; 
	int totalTasks[2] = { TOTAL_TASKS_PZERO , TOTAL_TASKS_PONE };

	do {flag[currentID] = true;
		turn = currentID;
		while (flag[otherID] && turn == currentID)
			/* Spinlock */;
		sharedResource++;
		taskNumber++;
		printf("Thread %d completed task number: %d. \n", currentID, taskNumber);
		flag[currentID] = false;
	} while (taskNumber < totalTasks[currentID]);

	if (finishLine) {
		printf("Thread %d finnished FIRST.\n", currentID);
		finishLine = false;
	} else {
		printf("Thread %d finnished SECOND.\n", currentID);
	}
	/* Remainder */
}

void Dekker(int currentID) {
	int otherID = (currentID) ? 0 : 1; // Get the ID of the other thread.
	int taskNumber = 0; 
	int totalTasks[2] = { TOTAL_TASKS_PZERO , TOTAL_TASKS_PONE };

	do {
		flag[currentID] = true;

		while (flag[otherID]) {
			if (turn == otherID) {
				flag[currentID] = false;
				while (turn == otherID)
					/* Spinlock */;
				flag[currentID] = true;
			}
		}
		/* Critical Section */
		taskNumber++;
		sharedResource++; 
		printf("Thread %d completed task number: %d. \n", currentID, taskNumber);
		turn = otherID;
		flag[currentID] = false;
	} while (taskNumber < totalTasks[currentID]);

	if (finishLine) {
		printf("Thread %d finished FIRST.\n", currentID);
		finishLine = false;
	} else {
		printf("Thread %d finished SECOND.\n", currentID);
	}

}

int main() {
	cout << "Ready.. get set... GO!" << endl;
	printf("Initiating Peterson's Algorithm:\n");
	thread thread1(Peterson, 0);
	thread thread2(Peterson, 1);

	thread1.join();
	thread2.join();

	cout << endl;
	cout << "PETERSON RESULTS:\n";
	cout << "\tThread 0 total number of tasks: " << TOTAL_TASKS_PZERO << endl;
	cout << "\tThread 1 total number of tasks: " << TOTAL_TASKS_PONE << endl;
	cout << "\tCritical section tasks complete: " << sharedResource;
	cout << endl << endl;
	sharedResource = 0;
	finishLine = true;

	printf("Initiating Dekker's Algorithm:\n");
	thread thread3(Dekker, 0);
	thread thread4(Dekker, 1);

	thread3.join();
	thread4.join();

	cout << endl;
	cout << "DEKKER'S RESULTS:\n";
	cout << "\tThread 0 total number of tasks: " << TOTAL_TASKS_PZERO << endl;
	cout << "\tThread 1 total number of tasks: " << TOTAL_TASKS_PONE << endl;
	cout << "\tCritical section tasks complete: " << sharedResource;
	cout << endl << endl;
	
	cout << endl;
	system("pause");
	return 0;
}
/*
Owen, Zachary
CPSC 351: OS Concepts
Professor McCarthy

Banker's Algorithm
*/

#include <iostream>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <chrono> // embedded system, real-time
#include <random>
using namespace std;

#define NUMBER_OF_REQUESTS 10
#define NUMBER_OF_RELEASES 5
#define REQUEST_AND_RELEASE_CAPACITY 5

#define NUMBER_OF_CUSTOMERS 5 // value must be >= 0
#define NUMBER_OF_RESOURCES 3 // value must be >= 0

mutex mutualExclude;
mutex releasing;

int available[NUMBER_OF_RESOURCES] = { 0 };
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = { 5, 5, 5,
														  5, 5, 5,
														  5, 5, 5,
														  5, 5, 5, 
														  5, 5, 5 };
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = { 0 };
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = { 0 };

int request_resources(int customer_num, int request[]);
int release_resources(int customer_num, int release[]);
bool safetyAlgorithm(int customerID, int request[]);

void calculateNeed();
void customer(int customerID, bool req);

void print(int arr[], int size);
void print(int arr[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES]);

int main(int argc, char* argv[])
{
	srand(time(NULL));
	/* User passes the amount of instances each resource has as arguments. */
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
		available[i] = atoi(argv[i + 1]);
	
	/* Creates an ID number for all customers. */
	int customerID[NUMBER_OF_CUSTOMERS];
	for (int id = 0;  id < NUMBER_OF_CUSTOMERS; id++)
		customerID[id] = id;

	/* Sets need = maximum - allocation */
	calculateNeed();

	thread* customerCall[NUMBER_OF_REQUESTS]; 
	int idNum = 0, requestNum = 0;
	while (requestNum < NUMBER_OF_REQUESTS)
	{

		if (idNum == NUMBER_OF_CUSTOMERS)
			idNum = 0;

		thread* newThread = new thread(customer, customerID[idNum], true);

		/* Store memory location of threads to join main after loop*/
		customerCall[requestNum] = newThread;

		requestNum++;
		idNum++;
	}

	/* Where threads join parent. */
	for (int i = 0; i < NUMBER_OF_REQUESTS; i++)
		customerCall[i]->join();

	/* Struggled with executing request and release concurrently (so separated). I think I need chapter 5 algorithm to accomplish that. */
	printf("\nNow Releasing:\n");
	printf("Current Available: ");
	print(available, NUMBER_OF_RESOURCES);
	printf("Current Allocation:\n");
	print(allocation);

	thread* customerRelease[NUMBER_OF_RELEASES];
	int releaseNum = 0;
	idNum = 0;
	while (releaseNum < NUMBER_OF_RELEASES)
	{
		if (idNum == NUMBER_OF_CUSTOMERS)
			idNum = 0;

		thread* newThread = new thread(customer, customerID[idNum], false);
		customerRelease[releaseNum] = newThread;

		releaseNum++;
		idNum++;
	}
	for (int i = 0; i < NUMBER_OF_RELEASES; i++)
		customerRelease[i]->join();

	cout << endl;
	system("pause");
	return 0;
}

void customer(int customerID, bool req)
{
	/* Random number generation*/
	random_device rd;
	mt19937 mt(rd());
	uniform_int_distribution<int> distribution(0, REQUEST_AND_RELEASE_CAPACITY);

	/* Creates a customer request and release. */
	int request[NUMBER_OF_RESOURCES], release[NUMBER_OF_RESOURCES];
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		request[i] = distribution(mt);
		release[i] = distribution(mt);
	}
	int pass = 0;

	/* If requesting or releasing */
	if (req)
		pass = request_resources(customerID, request);
	else
		pass = release_resources(customerID, release);
}

int request_resources(int customer_num, int request[]) // Request Algorithm
{

	bool pass = true;
	int i = 0;
	while (pass && i < NUMBER_OF_RESOURCES) {	// Step 1
		if (request[i] <= need[customer_num][i])
			i++;
		else
			pass = false;
	}

	if (pass) {								// Step 2
		i = 0;
		while (pass && i < NUMBER_OF_RESOURCES) {
			if (request[i] <= available[i])
				i++;
			else
				pass = false;
		}
	}


	if (pass) {

		mutualExclude.lock();
		if (safetyAlgorithm(customer_num, request)) { // Mutual exclusion to prevent race conditions
			for (int resource = 0; resource < NUMBER_OF_RESOURCES; resource++) {
				available[resource] -= request[resource];
				allocation[customer_num][resource] += request[resource];
				need[customer_num][resource] -= request[resource];
			}
			printf("Request Granted for customer %d: ", customer_num);
			print(request, NUMBER_OF_RESOURCES);

			printf("Available: ");
			print(available, NUMBER_OF_RESOURCES);
			printf("Allocation:\n");
			print(allocation);
			printf("Need:\n");
			print(need);
			printf("\n");

		} else {
			printf("Request for customer %d would create an unsafe state: ", customer_num);
			print(request, NUMBER_OF_RESOURCES);
			pass = false;
		}
		mutualExclude.unlock();
	}

	return (pass - 1); // return 0 if granting request, -1 if rejecting.
}

bool safetyAlgorithm(int customerID, int request[]) 
{
	int allocationTest[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
	int needTest[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

	bool Finish[NUMBER_OF_CUSTOMERS] = { false };	 // Step 1: Create work and finish. Let work = available
	int work[NUMBER_OF_RESOURCES];
	for (int resource = 0; resource < NUMBER_OF_RESOURCES; resource++) { // Pretend by storing into new arrays, so not to modify actual arrays.
		work[resource] = available[resource] - request[resource]; 

		for (int customer = 0; customer < NUMBER_OF_CUSTOMERS; customer++) {
			allocationTest[customer][resource] = allocation[customer][resource];

			needTest[customer][resource] = need[customer][resource];
		}

		allocationTest[customerID][resource] += request[resource];
		needTest[customerID][resource] -= request[resource];

	}

	bool unsafe = false, safe = false;				// Prepration for Step 2
	int idx = 0, amountFreed = 0, prevFreed = 0;
	bool needLessThanWork = true;

	while (!unsafe && !safe) {
		if (!Finish[idx]) {						// Step 2 a
			int numOfInstances = 0;
			while (numOfInstances < NUMBER_OF_RESOURCES && needLessThanWork) {
				if (needTest[idx][numOfInstances] > work[numOfInstances])
					needLessThanWork = false;
				else
					numOfInstances++;
			}

			if (needLessThanWork) {
				for (int resource = 0; resource < NUMBER_OF_RESOURCES; resource++)
					work[resource] += allocationTest[idx][resource];

				amountFreed++;

				Finish[idx] = true;
			}
			else
				needLessThanWork = true;
		}

		idx++;
	
		if (amountFreed == NUMBER_OF_CUSTOMERS)
			safe = true;

		if (idx == NUMBER_OF_CUSTOMERS) { // After checking each process.
			idx = 0;

			if (amountFreed - prevFreed == 0)
				unsafe = true;
				
			prevFreed = amountFreed;
		}

	}
	return safe;
}

int release_resources(int customer_num, int release[])
{
	bool releaseLessThanOrEqualToAllocation = true; // Prevents allocation from being less than zero.
	int i = 0;
	while (releaseLessThanOrEqualToAllocation && i < NUMBER_OF_RESOURCES) {
		if (release[i] <= allocation[customer_num][i])
			i++;
		else
			releaseLessThanOrEqualToAllocation = false;
	}


	if (releaseLessThanOrEqualToAllocation) {
		for (int resource = 0; resource < NUMBER_OF_RESOURCES; resource++) {
			available[resource] += release[resource];
			allocation[customer_num][resource] -= release[resource];
			need[customer_num][resource] += release[resource];
		}
		releasing.lock();
		printf("RELEASE Granted for customer %d: ", customer_num);
		print(release, NUMBER_OF_RESOURCES);
		printf("Available now: ");
		print(available, NUMBER_OF_RESOURCES);
		printf("Allocation now:\n");
		print(allocation);
		releasing.unlock();

	}

	return (releaseLessThanOrEqualToAllocation - 1); // return 0 if granting release. -1 if not.
}


void calculateNeed()
{
	for (int customer = 0; customer < NUMBER_OF_CUSTOMERS; customer++) {
		for (int resource = 0; resource < NUMBER_OF_RESOURCES; resource++) {
			need[customer][resource] = maximum[customer][resource] - allocation[customer][resource];
		}
	}
}


void print(int arr[], int size)
{
	for (int i = 0; i < size; i++){
		printf("%d ", arr[i]);
	}
	printf("\n");
}

void print(int arr[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES])
{
	for (int row = 0; row < NUMBER_OF_CUSTOMERS; row++) {
		printf("\t");
		for (int col = 0; col < NUMBER_OF_RESOURCES; col++) {
			printf("%d ", arr[row][col]);
		}
		printf("\n");
	}
}
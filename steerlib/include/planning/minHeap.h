#ifndef BINARYHEAP_H
#define BINARYHEAP_H

#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

struct state {
	float cost;
	int x, y;
};

class minHeap {
private:
	vector <state> heap;
	int leftChild(int parent);
	int rightChild(int parent);
	int parent(int child);
public:
	void displayHeap(); // Helper function
	int elementInHeap(int xCoord, int yCoord);
	void Remove(int index);
	state pop();
	int size();
	void min_heapify_after_pop(int index);
	void min_heapify_after_insert(int index);
	void Insert(int xCoord, int yCoord, float cost);
	bool IsEmpty();
};

#endif
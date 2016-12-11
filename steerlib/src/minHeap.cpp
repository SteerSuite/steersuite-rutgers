#include "planning/minHeap.h"

using namespace std;
#include <iostream>
#include <cstdlib>
#include <vector>

bool minHeap::IsEmpty(){
	return heap.size()==0;
}

int minHeap::size() {
	return heap.size();
}

int minHeap::elementInHeap(int xCoord, int yCoord) {
	for (int i = 0; i < heap.size(); i++) {
		if (heap.at(i).x == xCoord && heap.at(i).y == yCoord) 
			return i;
	}
	return -1;
}

void minHeap::Remove(int index) {
	if (index == heap.size()-1) {
		heap.pop_back();
		return;
	}
	swap(heap.at(index),heap.at(heap.size()-1));
	heap.pop_back();
	if (index == 0 || heap.at(index).cost > heap.at(parent(index)).cost) {
		min_heapify_after_pop(index);
	}
	else
		min_heapify_after_insert(index);
}

void minHeap::Insert(int xCoord, int yCoord, float cost) {
	state temp;
	float min=99999.0;
	int index;

	temp.x = xCoord;
	temp.y = yCoord;
	temp.cost = cost;

	heap.push_back(temp);

	min_heapify_after_insert(heap.size()-1);

	state current;
	for(int i=0;i<heap.size();i++){
		current = heap.at(i);
		if(current.cost<min){
			min=current.cost;
			index=i;
		}
	}
	current=heap.at(0);
	if(current.cost > min){
		cout<<current.cost<<" "<<min<<endl;
		cout<<"heap compromised at "<<index<<" with size = "<<heap.size()<<endl;
	}
}

state minHeap::pop() {
	state temp = heap.front();

	swap(heap.at(0),heap.at(heap.size()-1));

	heap.pop_back();

	min_heapify_after_pop(0);

	return temp;
}

void swap(state* a, state* b) {
	state temp;

	temp.x = a->x;
	temp.y = a->y;
	temp.cost = a->cost;

	a->x = b->x;
	a->y = b->y;
	a->cost = b->cost;

	b->x = temp.x;
	b->y = temp.y;
	b->cost = temp.cost;
}

int minHeap::leftChild(int parent) {
	int leftChildIndex = (2*parent) + 1;
	if (leftChildIndex < heap.size()) 
		return leftChildIndex;
	else
		return -1;
}

int minHeap::rightChild(int parent) {
	int rightChildIndex = (2*parent+1) + 1;
	if (rightChildIndex < heap.size())
		return rightChildIndex;
	else
		return -1;
}

int minHeap::parent(int child) {
	int parentIndex = (child-1)/2;
	if (parentIndex < 0)
		return -1;
	else
		return parentIndex;
}

void minHeap::min_heapify_after_pop(int index) {
	int leftChildIndex = leftChild(index);
	int rightChildIndex = rightChild(index);
	int minElementIndex = index;

	if (leftChildIndex < heap.size() && heap.at(leftChildIndex).cost < heap.at(index).cost) 
		minElementIndex = leftChildIndex;

	if (rightChildIndex < heap.size() && heap.at(rightChildIndex).cost < heap.at(minElementIndex).cost)
		minElementIndex = rightChildIndex;

	if (minElementIndex != index) {
		swap(heap.at(index), heap.at(minElementIndex));
		min_heapify_after_pop(minElementIndex);
	}
}

void minHeap::min_heapify_after_insert(int index) {
	int parentIndex = parent(index);
	if (parentIndex >= 0 && heap.at(parentIndex).cost > heap.at(index).cost) {
		swap(heap.at(index),heap.at(parentIndex));
		min_heapify_after_insert(parentIndex);
	}
}

void minHeap::displayHeap() {
	cout<<"Printing Heap Contents..."<<endl;
	for (int i = 0; i < heap.size(); i++) 
		cout<<heap[i].x<<" "<<heap[i].y<<" Cost: "<<heap[i].cost<<endl;
}
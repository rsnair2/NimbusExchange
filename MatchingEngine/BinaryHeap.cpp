#include "BinaryHeap.h"

// compute children/parent index related
template <typename T>
int BinaryHeap<T>::getParentIndex(int nodeIndex) {
	if(nodeIndex == 0)
		return -1;
	return (nodeIndex - 1)/2;
}

template <typename T>
int BinaryHeap<T>::getLeftChildIndex(int nodeIndex) {
	int idx = 2*nodeIndex + 1;
	return idx < heap.size() ? idx : -1;
}

template <typename T>
int BinaryHeap<T>::getRightChildIndex(int nodeIndex) {
	int idx = 2*nodeIndex + 2;
	return idx < heap.size() ? idx : -1;
}

template <typename T>
void BinaryHeap<T>::swap(int idx1, int idx2) {
	BinaryHeapNode * tmp = heap[idx1];
	heap[idx1] = heap[idx2];
	heap[idx2] = tmp;
	idToIndexMap[heap[idx1]->id] = idx1;
	idToIndexMap[heap[idx2]->id] = idx2;
}


// heapify related
template <typename T>
void BinaryHeap<T>::heapifyUp(int nodeIndex) {
	int parentIndex = getParentIndex(nodeIndex);
	while(parentIndex != -1) {
		T parent = heap[parentIndex]->payload;
		T current = heap[nodeIndex]->payload;
		if(compare(current, parent)) {
			swap(parentIndex, nodeIndex);
			nodeIndex = parentIndex;
			parentIndex = getParentIndex(nodeIndex);	
		}
		else {
			break;
		}
	}
}

template <typename T>
void BinaryHeap<T>::heapifyDown(int nodeIndex) {
	int leftChildIndex = getLeftChildIndex(nodeIndex);
	int rightChildIndex = getRightChildIndex(nodeIndex);
	T leftChildElem, rightChildElem, currentNodeElem;

	while(leftChildIndex != -1) {
		if(rightChildIndex == -1) {
			rightChildIndex = leftChildIndex;
		}

		leftChildElem = heap[leftChildIndex]->payload;
		rightChildElem = heap[rightChildIndex]->payload;
		currentNodeElem = heap[nodeIndex]->payload;

		int higherPriorityChildIndex = compare(leftChildElem, rightChildElem) ? leftChildIndex : rightChildIndex;
		T higherPriorityChildElem = compare(leftChildElem, rightChildElem) ? leftChildElem : rightChildElem;

		if(compare(higherPriorityChildElem, currentNodeElem)) {
			swap(higherPriorityChildIndex, nodeIndex);
			nodeIndex = higherPriorityChildIndex;
		}
		else
			break;

		leftChildIndex = getLeftChildIndex(nodeIndex);
		rightChildIndex = getRightChildIndex(nodeIndex);
	}

}

// public interface functions
template <typename T>
void BinaryHeap<T>::insert(T elem, int id) {
	BinaryHeapNode * newNode = new BinaryHeapNode(elem, id);
	heap.push_back(newNode);
	idToIndexMap[id] = heap.size() - 1;
	heapifyUp(heap.size() - 1);
}

template <typename T>
T BinaryHeap<T>::rootElem() {
	return heap[0]->payload;
}

template <typename T>
void BinaryHeap<T>::deleteRootElem() {
	swap(0, heap.size() - 1);
	BinaryHeapNode * prevRoot = heap[heap.size() - 1];
	heap.pop_back();
	idToIndexMap.erase(prevRoot->id);
	free(prevRoot);
	heapifyDown(0);
}

template <typename T>
T & BinaryHeap<T>::operator[](int i) {
	int idx = idToIndexMap[i];
	return heap[idx]->payload;
}

template <typename T>
void BinaryHeap<T>::replaceElemWithId(int id, T newElem) {
	int idx = idToIndexMap[id];
	heap[idx]->payload = newElem;
	fixTreeAtElemWithId(id);
}


template <typename T>
void BinaryHeap<T>::fixTreeAtElemWithId(int id) {
	int idx = idToIndexMap[id];
	heapifyUp(idx);
	heapifyDown(idx);
}

template <typename T>
void BinaryHeap<T>::deleteElemWithId(int id) {
	int idx = idToIndexMap[id];
	swap(idx, heap.size() - 1);
	BinaryHeapNode * prevHeapNode = heap[heap.size() - 1];
	heap.pop_back();
	idToIndexMap.erase(prevHeapNode->id);
	free(prevHeapNode);
	heapifyDown(idx);
}

template class BinaryHeap<int>;


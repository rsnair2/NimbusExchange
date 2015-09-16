#include <iostream>
#include <vector>
#include <map>


using namespace std;

template <typename T>
class BinaryHeap {

private:

	class BinaryHeapNode {
	public:
		T payload;
		int id;

		BinaryHeapNode(T payload, int id) {
			this->id = id;
			this->payload = payload;
		}

		friend ostream & operator<<(ostream & os, const BinaryHeapNode & node) {
			cout << (node.payload);
		}
	};

	void heapifyUp(int nodeIndex);
	void heapifyDown(int nodeIndex);
	int getParentIndex(int nodeIndex);
	int getLeftChildIndex(int nodeIndex);
	int getRightChildIndex(int nodeIndex);
	vector<BinaryHeapNode *> heap;
	void swap(int idx1, int idx2);
	map<int, int> idToIndexMap;
	bool (*compare)(T, T);	// need to return true if a is more important than b
	
public:
	T rootElem();
	void insert(T elem, int id); // unique id is needed for every node if you want to use modifyElemWithId feature
	void deleteRootElem();
	int size() { return heap.size(); };
	void replaceElemWithId(int id, T newElem);
	void fixTreeAtElemWithId(int id);
	void deleteElemWithId(int id);

	T operator[](int i);

	friend ostream & operator<<(ostream & os, const BinaryHeap & heap) {
		int numberOfElementsAtLevel = 1;
		int counter = 0;

		for(int i = 0; i < heap.heap.size(); i++) {
			os << *(heap.heap[i]->payload);
			os << " ";
			counter++;
			if(counter == numberOfElementsAtLevel) {
				counter = 0;
				numberOfElementsAtLevel *= 2;
				os << endl;
			}
		}
		os << endl;
		return os;
	}

	BinaryHeap(bool (*f)(T, T)) {
		compare = f;
	};
};

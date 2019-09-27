#pragma once

#include <forward_list>
#include <queue>
#include <iterator>

namespace PixCraft {
	template<typename T>
	struct PairingHeapNode {
		T root;
		std::forward_list<PairingHeapNode<T>> subheaps;
	};
	
	template<typename T>
	class PairingHeapIterator {
	public:
		PairingHeapIterator() = default;
		PairingHeapIterator(PairingHeapNode<T>* start);
		
		bool done();
		
		T& operator*();
		T* operator->();
		PairingHeapIterator& operator++();
		
	private:
		std::queue<PairingHeapNode<T>*> queue;
	};
	
	template<typename T, typename Compare = std::less<T>>
	class PairingHeap {
	public:
		PairingHeap();
		PairingHeap(const PairingHeap& other);
		~PairingHeap();
		
		bool empty();
		uint32_t size();
		
		T& min();
		PairingHeapIterator<T> iter();
		
		void insert(T element);
		void removeMin();
		
	private:
		Compare comp;
		PairingHeapNode<T>* node;
		uint32_t count;
		
		void meldWith(PairingHeap& other);
	};
}

#include "pairing_heap_impl.hpp"

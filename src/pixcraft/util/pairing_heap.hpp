#pragma once

#include <forward_list>

namespace PixCraft {
	template<typename T>
	struct PairingHeapNode {
		T root;
		std::forward_list<PairingHeapNode<T>> subheaps;
	};
	
	template<typename T, typename Compare = std::less<T>>
	class PairingHeap {
	public:
		PairingHeap();
		PairingHeap(const PairingHeap& other);
		~PairingHeap();
		
		bool empty();
		T& min();
		std::vector<T> data();
		
		void insert(T element);
		void deleteMin();
		
	private:
		Compare comp;
		PairingHeapNode<T>* node;
		
		void meldWith(PairingHeap& other);
	};
}

#include "pairing_heap_impl.hpp"

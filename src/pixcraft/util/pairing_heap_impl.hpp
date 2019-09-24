#pragma once

#include <stdexcept>

namespace PixCraft {
	template<typename T, typename Compare>
	PairingHeap<T, Compare>::PairingHeap() : node(nullptr) {}

	template<typename T, typename Compare>
	PairingHeap<T, Compare>::PairingHeap(const PairingHeap<T, Compare>& other)
		: node(other.node ? new PairingHeapNode<T>(*other.node) : nullptr) {}

	template<typename T, typename Compare>
	PairingHeap<T, Compare>::~PairingHeap() {
		if(node) {
			delete node;
		}
	}

	template<typename T, typename Compare>
	bool PairingHeap<T, Compare>::empty() { return !node; }

	template<typename T, typename Compare>
	T& PairingHeap<T, Compare>::min() {
		if(empty()) throw std::logic_error("Cannot get the min of empty pairing heap");
		return node->root;
	}
	
	template<typename T, typename Compare>
	std::vector<T> PairingHeap<T, Compare>::data() {
		std::vector<T> res;
		std::vector<PairingHeapNode<T>*> stack;
		if(node)
			stack.push_back(node);
		while(!stack.empty()) {
			PairingHeapNode<T>* node2 = stack.back();
			stack.pop_back();
			res.push_back(node2->root);
			for(auto it = node2->subheaps.begin(), end = node2->subheaps.end(); it != end; ++it) {
				stack.push_back(&*it);
			}
		}
		return res;
	}

	template<typename T, typename Compare>
	void PairingHeap<T, Compare>::insert(T element) {
		PairingHeap<T, Compare> newHeap;
		newHeap.node = new PairingHeapNode<T> {
			element,
			std::forward_list<PairingHeapNode<T>>()
		};
		meldWith(newHeap);
	}

	template<typename T, typename Compare>
	void PairingHeap<T, Compare>::deleteMin() {
		throw std::logic_error("Not yet implemented");
	}

	template<typename T, typename Compare>
	void PairingHeap<T, Compare>::meldWith(PairingHeap<T, Compare>& other) {
		if(other.empty()) {
			// nothing to do
		} else if(empty()) { // just copy other's contents
			node = new PairingHeapNode<T>(*other.node);
		} else if(comp(node->root, other.node->root)) {
			auto newNode = new PairingHeapNode<T> {
				node->root,
				std::forward_list<PairingHeapNode<T>>(node->subheaps)
			};
			newNode->subheaps.push_front(*other.node);
			delete node;
			node = newNode;
		} else {
			auto newNode = new PairingHeapNode<T> {
				other.node->root,
				std::forward_list<PairingHeapNode<T>>(other.node->subheaps)
			};
			newNode->subheaps.push_front(*node);
			delete node;
			node = newNode;
		}
	}
}

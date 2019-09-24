#pragma once

#include <stdexcept>
#include <iterator>
#include <memory>

namespace PixCraft {
	template<typename T, typename Compare>
	PairingHeapNode<T>* meldHeapNodes(PairingHeapNode<T>& node1, PairingHeapNode<T>& node2, Compare& comp) {
		if(comp(node1.root, node2.root)) {
			auto newNode = new PairingHeapNode<T> {
				node1.root,
				std::forward_list<PairingHeapNode<T>>(node1.subheaps)
			};
			newNode->subheaps.push_front(node2);
			return newNode;
		} else {
			auto newNode = new PairingHeapNode<T> {
				node2.root,
				std::forward_list<PairingHeapNode<T>>(node1.subheaps)
			};
			newNode->subheaps.push_front(node1);
			return newNode;
		}
	}
	
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
	void PairingHeap<T, Compare>::removeMin() {
		if(empty()) throw std::logic_error("Cannot remove the min of empty pairing heap");
		std::vector<std::unique_ptr<PairingHeapNode<T>>> meldedPairs;
		auto it = node->subheaps.begin();
		auto end = node->subheaps.end();
		while(it != end) {
			PairingHeapNode<T>& first = *it;
			++it;
			if(it == end) { // 'first' is unpaired
				meldedPairs.emplace_back(new PairingHeapNode<T>(first));
			} else {
				PairingHeapNode<T>& second = *it;
				meldedPairs.emplace_back(meldHeapNodes(first, second, comp));
			}
		}
		delete node;
		node = nullptr;
		for(auto it = std::make_move_iterator(meldedPairs.rbegin()), end = std::make_move_iterator(meldedPairs.rend()); it != end; ++it) {
			std::unique_ptr<PairingHeapNode<T>> node2 = *it; // move unique_ptr to local variable
			if(node) {
				auto newNode = meldHeapNodes(*node, *node2, comp);
				delete node;
				node = newNode;
			} else {
				node = node2.release();
			}
		}
	}

	template<typename T, typename Compare>
	void PairingHeap<T, Compare>::meldWith(PairingHeap<T, Compare>& other) {
		if(other.empty()) {
			// nothing to do
		} else if(empty()) { // just copy other's contents
			node = new PairingHeapNode<T>(*other.node);
		} else {
			auto newNode = meldHeapNodes(*node, *other.node, comp);
			delete node;
			node = newNode;
		}
	}
}

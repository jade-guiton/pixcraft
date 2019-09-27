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
				std::forward_list<PairingHeapNode<T>>(node2.subheaps)
			};
			newNode->subheaps.push_front(node1);
			return newNode;
		}
	}
	
	template<typename T>
	PairingHeapIterator<T>::PairingHeapIterator(PairingHeapNode<T>* start) {
		queue.push(start);
	}
	
	template<typename T>
	bool PairingHeapIterator<T>::done() {
		return queue.empty();
	}
	
	template<typename T>
	T& PairingHeapIterator<T>::operator*() {
		return queue.front()->root;
	}
	
	template<typename T>
	T* PairingHeapIterator<T>::operator->() {
		return &queue.front()->root;
	}
	
	template<typename T>
	PairingHeapIterator<T>& PairingHeapIterator<T>::operator++() {
		for(PairingHeapNode<T>& node : queue.front()->subheaps) {
			queue.push(&node);
		}
		queue.pop();
		return *this;
	}
	
	template<typename T, typename Compare>
	PairingHeap<T, Compare>::PairingHeap() : node(nullptr), count(0) {}

	template<typename T, typename Compare>
	PairingHeap<T, Compare>::PairingHeap(const PairingHeap<T, Compare>& other)
		: node(other.node ? new PairingHeapNode<T>(*other.node) : nullptr), count(other.count) {}

	template<typename T, typename Compare>
	PairingHeap<T, Compare>::~PairingHeap() {
		if(node) {
			delete node;
		}
	}

	template<typename T, typename Compare>
	bool PairingHeap<T, Compare>::empty() { return !node; }
	
	template<typename T, typename Compare>
	uint32_t PairingHeap<T, Compare>::size() { return count; }

	template<typename T, typename Compare>
	T& PairingHeap<T, Compare>::min() {
		if(empty()) throw std::logic_error("Cannot get the min of empty pairing heap");
		return node->root;
	}
	
	template<typename T, typename Compare>
	PairingHeapIterator<T> PairingHeap<T, Compare>::iter() {
		if(node)
			return PairingHeapIterator<T>(node);
		else
			return PairingHeapIterator<T>();
	}

	template<typename T, typename Compare>
	void PairingHeap<T, Compare>::insert(T element) {
		PairingHeap<T, Compare> newHeap;
		newHeap.node = new PairingHeapNode<T> {
			element,
			std::forward_list<PairingHeapNode<T>>()
		};
		meldWith(newHeap);
		++count;
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
				++it;
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
		--count;
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
		count += other.count;
	}
}

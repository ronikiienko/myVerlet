#pragma once

#include <vector>
#include <stdexcept>

class SparseSet {
private:
    std::vector<int> dense;
    std::vector<int> sparse;
    int maxIndex;
    int realSize = 0;
public:
    explicit SparseSet(int maxIndex) : maxIndex(maxIndex) {
        dense.reserve(maxIndex + 1);
        sparse.resize(maxIndex + 1, -1);
    }

    void insert(int index) {
        // TODO remove debug checks
        if (index > maxIndex || realSize >= maxIndex || index < 0) {
            throw std::runtime_error("ran out of sparse set capacity / inserting out of maxIndex range / inserting at index less than 0");
            return;
        }

        if (sparse[index] != -1) {
            throw std::runtime_error("Trying to insert element which is already in the set");
            return;
        }

        sparse[index] = realSize;
        if (realSize < dense.size()) {
            dense[realSize] = index;
        } else {
            dense.push_back(index);
        }
        realSize++;

    }

    void remove(int index) {
        if (index < 0 || index > maxIndex) {
            throw std::runtime_error("Index is smaller than 0 or greater than maxIndex") ;
            return;
        }
        if (sparse[index] == -1) {
            throw std::runtime_error("Trying to remove element which is already empty");
            return;
        }

        const int lastDenseIndex = realSize - 1;
        const int removedDenseIndex = sparse[index];
        const int lastSparseIndex = dense[lastDenseIndex];
        const int removedSparseIndex = index;

        if (removedDenseIndex != lastDenseIndex) {
            sparse[lastSparseIndex] = removedDenseIndex;
            dense[removedDenseIndex] = dense[lastDenseIndex];
        }

        sparse[removedSparseIndex] = -1;
        realSize--;
    }

    bool search(int index) {
        return sparse[index] != -1;
    }

    [[nodiscard]] int size() const {
        return realSize;
    }

    template<typename T>
    void forEach(const T& callback, int start = 0, int end = -1) {
        if (end == -1) {
            end = size();
        }
        for (int i = start; i < end; i++) {
            callback(dense[i]);
        }
    }
};
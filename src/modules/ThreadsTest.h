#include <iostream>
#include <vector>
#include <thread>
#include <numeric>
#include <chrono>

long long sumPortion(const std::vector<int> &nums, int start, int end) {
    return std::accumulate(nums.begin() + start, nums.begin() + end, 0LL);
}

void testThreads(int numThreads) {
    const int SIZE = 1000000000;
    std::vector<int> nums(SIZE, 1);

    std::vector<std::thread> threads;
    std::vector<long long> results(numThreads);

    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numThreads; i++) {
        int start = (SIZE / numThreads) * i;
        int end = (SIZE / numThreads) * (i + 1);
        threads.push_back(std::thread(
                [&results, &nums, start, end, i] {
                    results[i] = sumPortion(nums, start, end);
                }
        ));
    }


    for (std::thread &thread: threads) {
        thread.join();
    }

    long long finalSum = std::accumulate(results.begin(), results.end(), 0LL);

    auto stopTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime);


    std::cout << "Time taken: " << duration.count() << " microseconds; " << "Num threads: " << numThreads << std::endl;
    std::cout << "Final sum: " << finalSum << std::endl;
}

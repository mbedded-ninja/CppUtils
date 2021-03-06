///
/// \file 				ThreadSafeQueueTests.cpp
/// \author 			Geoffrey Hunter (www.mbedded.ninja) <gbmhunter@gmail.com>
/// \edited             n/a
/// \created			2017-08-11
/// \last-modified		2017-10-06
/// \brief 				Contains tests for the ThreadSafeQueue class.
/// \details
///		See README.md in root dir for more info.

// System includes
// nothing

// 3rd party includes
#include <thread>
#include "gtest/gtest.h"

// User includes
#include "CppUtils/ThreadSafeQueue.hpp"

using namespace mn::CppUtils;

namespace {

    class ThreadSafeQueueTests : public ::testing::Test {
    protected:
        ThreadSafeQueueTests() {}
        virtual ~ThreadSafeQueueTests() {}
    };

    TEST_F(ThreadSafeQueueTests, SingleThreadPushPop) {
        ThreadSafeQueue<std::string> threadSafeQueue;
        threadSafeQueue.Push("hello");
        EXPECT_EQ(1, threadSafeQueue.Size());
        std::string output;
        bool retVal = threadSafeQueue.TryPop(output, std::chrono::milliseconds(0));
        EXPECT_EQ(true, retVal);
        EXPECT_EQ("hello", output);
    }

    TEST_F(ThreadSafeQueueTests, SingleThreadDoublePushPop) {
        ThreadSafeQueue<std::string> threadSafeQueue;
        threadSafeQueue.Push("hello");
        threadSafeQueue.Push("world");
        std::string output;
        bool retVal;
        EXPECT_EQ(2, threadSafeQueue.Size());
        retVal = threadSafeQueue.TryPop(output, std::chrono::milliseconds(0));
        EXPECT_EQ(true, retVal);
        EXPECT_EQ("hello", output);
        EXPECT_EQ(1, threadSafeQueue.Size());
        retVal = threadSafeQueue.TryPop(output, std::chrono::milliseconds(0));
        EXPECT_EQ(true, retVal);
        EXPECT_EQ("world", output);
    }

    TEST_F(ThreadSafeQueueTests, MultiThread) {

        static constexpr uint8_t NUM_THREADS = 10;
        static constexpr uint32_t NUM_ITEMS_PER_THREAD = 100;

        ThreadSafeQueue<uint8_t> threadSafeQueue;

        std::vector<std::thread> threads;
        for(uint8_t i = 0; i < NUM_THREADS; i++) {
            threads.push_back(std::thread([i, &threadSafeQueue]() {
               for(uint8_t j = 0; j < NUM_ITEMS_PER_THREAD; j++) {
                   std::this_thread::sleep_for(std::chrono::milliseconds(10));
                   threadSafeQueue.Push(i);
               }
            }));
        }

        std::array<uint8_t, NUM_THREADS> counts{};

        uint8_t output;

        for(int i = 0; i < NUM_THREADS*NUM_ITEMS_PER_THREAD; i++) {
            threadSafeQueue.Pop(output);
            counts[output]++;
        }

        for(auto count: counts) {
            EXPECT_EQ(NUM_ITEMS_PER_THREAD, count);
        }

        for(auto &thread: threads) {
            thread.join();
        }
    }
}  // namespace
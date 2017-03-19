#pragma once

#include <cstdint>
#include <thread>
#include <atomic>
#include <vector>
#include <future>
#include <iostream>
#include "rcu_domain.hpp"

thread_local int tl_urcu_rv_tid = -1;

/**
 * This is URCU Reader's Version, a Userspace RCU that uses only the C++
 * memory model and atomics, and that allows multiple updaters to share
 * a grace period without having to hold a lock.
 * Based on the original algorithm by Correia and Ramalhete described in
 * the paper "Correct traversal of Lazy Lists".
 *
 * Reader registration works by doing a CAS in readersVersion[i] from UNASSIGNED
 * to NOT_READING state. Threads always start from i=0 and move forward.
 * I'm sure we can come up with a more flexible ReadIndicator that at the same
 * time allows for registration, but for the time being its ok.
 *
 * Limitations:
 * - read_lock()/read_unlock() are not reentrant;
 * - The number of registered (reader) threads can not be larger than maxThreads
 * - The retire()/barrier() implementation uses futures and async for simplicity but it is crappy and untested.
 *
 *
 */
class rcu_domain_rv {

    static const int CLPAD = (128/sizeof(uint64_t));
    static const uint64_t NOT_READING = 0xFFFFFFFFFFFFFFFE;
    static const uint64_t UNASSIGNED =  0xFFFFFFFFFFFFFFFD;

    const int maxThreads; // Defaults to 32
    std::atomic<uint64_t> reclaimerVersion alignas(128) = { 0 };
    std::atomic<uint64_t>* readersVersion alignas(128);
    std::mutex listMutex;
    std::vector<std::future<void>> futureList;

public:
    rcu_domain_rv(const int maxThreads=32): maxThreads{maxThreads}
    {
        readersVersion = new std::atomic<uint64_t>[maxThreads*CLPAD];
        for (int i=0; i < maxThreads; i++) {
            readersVersion[i*CLPAD].store(UNASSIGNED, std::memory_order_relaxed);
        }
    }

    ~rcu_domain_rv() {
        delete[] readersVersion;
    }

    void register_thread()
    {
        if (tl_urcu_rv_tid != -1) {
            std::cout << "Warning: calling register_thread() on an already registered thread\n";
            return;
        }
        for (int i=0; i < maxThreads; i++) {
            if (readersVersion[i*CLPAD].load() != UNASSIGNED) continue;
            uint64_t curr = UNASSIGNED;
            if (readersVersion[i*CLPAD].compare_exchange_strong(curr, NOT_READING)) {
                 tl_urcu_rv_tid = i;
                 return;
            }
        }
        std::cout << "Error: too many threads already registered\n";
    }

    void unregister_thread()
    {
        if (tl_urcu_rv_tid == -1) {
            std::cout << "Error: calling unregister_thread() from a thread that was never registered\n";
            return;
        }
        readersVersion[tl_urcu_rv_tid*CLPAD].store(UNASSIGNED);
    }

    void read_lock() noexcept
    {
        const int tid = tl_urcu_rv_tid;
        const uint64_t rv = reclaimerVersion.load();
        readersVersion[tid*CLPAD].store(rv);
        const uint64_t nrv = reclaimerVersion.load();
        if (rv != nrv) readersVersion[tid*CLPAD].store(nrv, std::memory_order_relaxed);
    }

    void read_unlock() noexcept
    {
        const int tid = tl_urcu_rv_tid;
        readersVersion[tid*CLPAD].store(NOT_READING, std::memory_order_release);
    }

    void synchronize() noexcept { synchronize_tid(); }

    void synchronize_tid(const int mytid = -1) noexcept
    {
        const int tid = (mytid == -1) ? tl_urcu_rv_tid : mytid;
        const uint64_t waitForVersion = reclaimerVersion.load()+1;
        auto tmp = waitForVersion-1;
        reclaimerVersion.compare_exchange_strong(tmp, waitForVersion);
        for (int i=0; i < maxThreads; i++) {
            // Handle the quiescent_state() case: if it's the same thread, just skip.
            // If there is an error in the program and we were called inside a
            // block of read_lock()/unlock() then this will cause errors.
            if (tid == i) continue;
            while (readersVersion[i*CLPAD].load() < waitForVersion) {  // spin
                // TODO: find a better way to spin... maybe spin a random number of iterations
                // and then call this_thread::yield() ?
            }
        }
        // TODO: Do we need to handle here the waiting callbacks?
    }

    void retire(rcu_head *rhp, void (*cbf)(rcu_head *rhp))
    {
        const int tid = tl_urcu_rv_tid;
        auto lamb = [&rhp,&cbf,&tid,this]() { synchronize_tid(tid); cbf(rhp); };
        std::future<void> fut = std::async(std::launch::async, lamb);
        std::lock_guard<std::mutex> lock(listMutex);
        futureList.push_back(std::move(fut));
    }

    void barrier() noexcept
    {
        std::lock_guard<std::mutex> lock(listMutex);
        for (auto& fut : futureList) fut.wait();
        futureList.clear();
    }

    void quiescent_state() noexcept { read_lock(); }
    void thread_offline() noexcept { read_unlock(); }
    void thread_online() noexcept { read_lock(); }

    static constexpr bool register_thread_needed() { return true; }
    static constexpr bool quiescent_state_needed() { return false; }
};

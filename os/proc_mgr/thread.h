#ifndef _THREAD_H
#define _THREAD_H

#include "cpu.h"
#include "mem_region.hpp"
#include <cstdint>

enum class ThreadState : uint8_t
{
    Created,
    Ready,
    Executing,
    Blocked,
    Zombie,
    Dead,
    NUM_STATES,
};

class Process;

class Thread
{
        friend class Process;

    private:
        uint32_t _threadId;
        Process* _parentProcess;

        ThreadState _state;

        bool _privileged;
        bool _useMainStack;
        SavedRegisters _cpuRegs;
        MemRegion _stack;

    public:
        /// @brief Included for flexibility, not intended for actually creating threads.
        /// Use Thread::Thread(Process&) instead.
        Thread();

        /// @brief Create a new Thread.
        /// @param parentProcess The parent process that is creating this thread.
        Thread(Process& parentProcess);

        /// @brief Included for flexibility, Threads are not meant to be copied.
        /// @param source The Thread from which to copy.
        Thread(const Thread& source);

        /// @brief Included for flexibility. Threads are not meant to be moved.
        /// @param source The Thread from which to move.
        Thread(Thread&& source);

        /// @brief Finalize a Thread.
        ~Thread();

        /// @brief Included for flexibility, Threads are not meant to be copied.
        /// @param source The Thread from which to copy.
        Thread& operator=(const Thread& source);

        /// @brief Included for flexibility. Threads are not meant to be moved.
        /// @param source The Thread from which to move.
        Thread& operator=(Thread&& source);

        uint32_t getId() const { return _threadId; };
        Process& getProcess() const { return *_parentProcess; };
        ThreadState getState() const { return _state; };
        bool isPrivileged() const { return _privileged; };
        bool isUsingMainStack() const { return _useMainStack; };
        const SavedRegisters& getSavedRegisters() const { return _cpuRegs; };
};

#endif

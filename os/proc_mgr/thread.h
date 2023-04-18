#ifndef _THREAD_H
#define _THREAD_H

#include "cpu.h"
#include "kernel_result_status.hpp"
#include "mem_mgr.h"
#include "mem_region.hpp"
#include "misc.hpp"
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
        SavedRegisters _savedRegs;
        MemRegion _stack;

    public:
        /// @brief Included for flexibility, not intended for actually creating threads.
        /// Use Thread::Thread(Process&) instead.
        Thread();

        /// @brief Create a new Thread.
        /// @param parentProcess The parent process that is creating this thread.
        /// @param memMgr The MemoryManager from which a stack will be allocated.
        Thread(Process& parentProcess, MemoryManager* memMgr);

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
        AutomaticallyStackedRegisters* GetStackedRegisters() const
        {
            // Stack pointer will be already pointing to the stacked R0 after an interrupt.
            return reinterpret_cast<AutomaticallyStackedRegisters*>(_savedRegs.SP);
        };
        const SavedRegisters* GetSavedRegisters() const { return &_savedRegs; };
        void SetSavedRegisters(const SavedRegisters& savedRegs) { _savedRegs = savedRegs; }

        /// @brief Sets the entry point of the thread upon beginning execution.
        /// @param startAddress The address of the first instruction to run.
        /// @return Whether the attempt to set the entry point was successful.
        KernelResultStatus SetEntryPoint(const VoidFunction startAddress);
};

#endif

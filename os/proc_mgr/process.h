#ifndef _PROCESS_H
#define _PROCESS_H

#include "doubly_linked_list.h"
#include "mem_mgr.h"
#include "mem_region.hpp"
#include "thread.h"
#include <cstdint>

#define MAX_MPU_REGIONS 8

using namespace os::utils::linked_list;

enum class ProcessState : uint8_t
{
    Created,
    Running,
    Blocked,
    Zombie,
    Traced,
    NUM_STATES,
};

/// @brief Represents a Process on the system.
class Process
{
    private:
        uint32_t _parentProcessId;
        uint32_t _processId;
        MemoryManager* _memMgr;

        ProcessState _state;
        bool _swapped;
        uint32_t _returnCode;

        DoublyLinkedList<MemRegion> _memRegionList;
        DoublyLinkedList<Thread> _threadList;

    public:
        /// @brief Included for flexibility, not intended for actually creating processes.
        /// Use Process::Process(const uint32_t, MemoryManager* const) instead.
        Process();
        /// @brief Create a new Process.
        /// @param parentProcessId ID of the parent process that is creating this one.
        /// @param memMgr MemoryManager from which this process can request memory allocations.
        Process(const uint32_t parentProcessId, MemoryManager* const memMgr);
        /// @brief Included for flexibility, Processes are not meant to be copied.
        /// @param other The Process from which to copy.
        Process(const Process& other);
        /// @brief Included for flexibility.
        /// @param other The Process from which to move.
        Process(Process&& other);
        /// @brief Finalize a Process.
        ~Process();
        /// @brief Included for flexibility, Processes are not meant to be copied.
        /// @param other The Process from which to copy.
        Process& operator=(const Process& other);
        /// @brief Included for flexibility.
        /// @param other The Process from which to move.
        Process& operator=(Process&& other);

        void ReadyForExec();
        void FinishExec();

        void SwapOut();
        void SwapIn();

        void Dispatch();
        void Suspend();

        void Sleep();
        void Wake();

        Thread* CreateThread();
        void DestroyThread(Thread* thread);

        void AddMemRegion(const MemRegion&);
};

#endif

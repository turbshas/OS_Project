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

class Process
{
    private:
        const uint32_t _parentProcessId;
        const uint32_t _processId;
        MemoryManager* const _memMgr;

        ProcessState _state;
        bool _swapped;
        uint32_t _returnCode;

        DoublyLinkedList<MemRegion> _memRegionList;
        DoublyLinkedList<Thread> _threadList;

    public:
        Process() = delete;
        Process(const uint32_t parentProcessId, MemoryManager* const memMgr);
        Process(const Process&);
        Process(Process&&);
        ~Process();
        Process& operator=(const Process&);
        Process& operator=(Process&&);

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

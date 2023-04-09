#ifndef _PROCESS_H
#define _PROCESS_H

#include <cstdint>

#include "doubly_linked_list.h"
#include "mem_region.hpp"
#include "thread.h"

#define MAX_MPU_REGIONS 8

class Process
{
public:
    enum class ProcessState;

private:
    uint32_t _parentProcessId;
    uint32_t _processId;
    ProcessState _state;
    bool _swapped;
    uint32_t _returnCode;

    DoublyLinkedList<MemRegion> _memRegionList;
    DoublyLinkedList<Thread *> _threadList;

public:
    enum class ProcessState
    {
        Created,
        Running,
        Blocked,
        Zombie,
        Traced,
        NUM_STATES,
    };

    Process();
    ~Process();

    void ReadyForExec();
    void FinishExec();

    void SwapOut();
    void SwapIn();

    void Dispatch();
    void Suspend();

    void Sleep();
    void Wake();

    Thread *CreateThread();
    void DestroyThread(Thread *thread);
};

#endif

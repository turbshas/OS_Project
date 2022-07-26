#ifndef _PROCESS_H
#define _PROCESS_H

#include <cstdint>

#include "doubly_linked_list.h"
#include "mpu.h"
#include "thread.h"

#define MAX_MPU_REGIONS 8

class Process {
    public:
        enum class ProcessState;

    private:
        uint32_t _parentProcessId;
        uint32_t _processId;
        ProcessState _state;
        bool _swapped;
        uint32_t _returnCode;

        DoublyLinkedList<mpu_region *> _memRegionList;
        DoublyLinkedList<Thread *> _threadList;

    public:
        enum class ProcessState {
            Created,
            Running,
            Blocked,
            Zombie,
            Traced,
            NUM_STATES,
        };

        Process();
        ~Process();

        void readyForExec();
        void finishExec();

        void swapOut();
        void swapIn();

        void dispatch();
        void suspend();

        void sleep();
        void wake();

        Thread *createThread();
        void destroyThread(Thread *thread);
};

#endif

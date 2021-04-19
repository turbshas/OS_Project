#ifndef _PROCESS_H
#define _PROCESS_H

#include <cstdint>
#include "mpu.h"
#include "thread.h"

class Process {
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

    private:
        uint32_t _parentProcessId;
        uint32_t _processId;
        ProcessState _state;
        bool _swapped;
        uint32_t _returnCode;

        uint8_t _activeRegions;
        mpu_region _memRegions[8]; /* TODO: make this a linked list as well? */

        uint32_t _numThreads;
        Thread *_firstThread;
        Thread *_lastThread;
};

#endif

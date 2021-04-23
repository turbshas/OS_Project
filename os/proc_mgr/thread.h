#ifndef _THREAD_H
#define _THREAD_H

#include <cstdint>
#include "cpu.h"

class Process;

class Thread {
    friend class Process;

    public:
        enum class ThreadState {
            Created,
            Ready,
            Executing,
            Blocked,
            Zombie,
            NUM_STATES,
        };

        Thread(Process &parentProcess);
        ~Thread();

        uint32_t getId()        const { return _threadId; };
        Process *getProcess()   const { return _parentProcess; };
        ThreadState getState()  const { return _state; };
        Thread *getPrev()       const { return _prev; };
        Thread *getNext()       const { return _next; };
        bool isPrivileged()     const { return _privileged; };
        bool isUsingMainStack() const { return _useMainStack; };
        void *getStackPointer() const { return _stack; };

    private:
        uint32_t _threadId;
        Process *_parentProcess;

        ThreadState _state; //TODO: figure out state machine

        Thread *_prev;
        Thread *_next;

        CpuRegsOnStack _regs;
        bool _privileged;
        bool _useMainStack;
        void *_stack;
};

#endif

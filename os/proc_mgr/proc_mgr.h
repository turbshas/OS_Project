#ifndef PROC_MGR_H
#define PROC_MGR_H

#include "mpu.h"

/* TODO:
 *   - Move cpu_regs_on_stack to cpu folder
 *   - Virtual round robin scheduling:
 *     - 1 Queue for processes that execute during entire time slice
 *     - 1 Queue for processes that block during time slice
 *     - Processes that block retain the amount of time left in slice
 *     - When a blocked process is next ran, it uses that amount as its next slice
 *     - Once slice runs out for blocked process, put it in regular queue
 *     - Queue for blocked processes has higher prio than regular queue
 *   - Some kind of processor affinity?
 *   - More queues when more than 1 core?
 *   - Queues for each core?
 *   - Knowing when to wake up threads/processes from sleep?
 *   - I/O operations?
 *
 *   - Create CPU class to represent hardware (place in cpu folder)
 *     - Should handle scheduling probably?
 *     - Maybe store some info about capabilities
 *     - Store general state (idle, busy, etc.)
 *     - Could go for performance and have scheduling domains
 */

struct cpu_regs_on_stack {
    uint32_t R4_11[8];
    uint32_t R0_3[4];
    uint32_t R12;
    uint32_t LR;
    uint32_t PC;
    uint32_t PSR;
};

class Process;

struct ThreadListMember {
    ThreadListMember *prev;
    ThreadListMember *next;
};

class Thread {
    public:
        enum class ThreadState {
            Created,
            Ready,
            Executing,
            Blocked,
            Zombie,
            NUM_STATES,
        };

    private:
        uint32_t thread_id;
        Process *owner_process;

        ThreadState state; //TODO: figure out state machine

        Thread *prev;
        Thread *next;

        cpu_regs_on_stack regs;
        void *stack;
};

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
        Process(const Process& other);
        ~Process();

        void readyForExec();
        void finishExec();

        void swapOut();
        void swapIn();

        void dispatch();
        void suspend();

        void sleep();
        void wake();

    private:
        uint32_t parent_process_id;
        uint32_t process_id;
        ProcessState state;
        bool swapped;
        uint32_t return_code;

        uint8_t regions_en;
        uint8_t padding[3];
        mpu_region mem_regions[8]; /* TODO: make this a linked list as well? */

        uint32_t num_threads;
        Thread *first_thread;
        Thread *last_thread;
};

#endif /* PROC_MGR_H */


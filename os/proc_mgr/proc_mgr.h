#ifndef PROC_MGR_H
#define PROC_MGR_H

/* TODO:
 *   - Move cpu_regs_on_stack to cpu folder
 *   - Make ProcessState/ThreadState classes with set of transition functions?
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

class Process {
    public:
        enum class ProcessState {
            CREATED,
            RUNNING,
            RUNNING_SWAPPED,
            STOPPED,
            STOPPED_SWAPPED,
            INTERRUPTIBLE,
            INTERRUPTIBLE_SWAPPED,
            UNINTERRUPTIBLE,
            UNINTERRUPTIBLE_SWAPPED,
            ZOMBIE,
            ZOMBIE_SWAPPED,
            TRACED,
            TRACED_SWAPPED,
        };

        class Thread {
            public:
                enum class ThreadState {
                    CREATED,
                    READY,
                    EXECUTING,
                    STOPPED,
                    INTERRUPTIBLE,
                    UNINTERRUPTIBLE,
                    ZOMBIE,
                };

            private:
                uint32_t thread_id;
                ThreadState state; //TODO: figure out state machine
                Process *owner_process;

                Thread *prev;
                Thread *next;

                cpu_regs_on_stack regs;
                void *stack;
        };

    private:
        uint32_t parent_process_id
        uint32_t process_id;
        ProcessState state;
        uint32_t return_code;

        uint8_t regions_en;
        uint8_t padding[3];
        mpu_region mem_regions[8]; /* TODO: make this a linked list as well? */

        uint32_t num_threads;
        Thread *first_thread;
        Thread *last_thread;
};

#endif /* PROC_MGR_H */


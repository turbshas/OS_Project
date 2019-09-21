#ifndef PROC_MGR_H
#define PROC_MGR_H

struct cpu_regs_on_stack {
    uint32_t R4_11[8];
    uint32_t R0_3[4];
    uint32_t R12;
    uint32_t LR;
    uint32_t PC;
    uint32_t PSR;
};

struct process;

struct thread {
    uint32_t thread_id;
    uint32_t state; //TODO: figure out state machine
    struct process *owner_process;
    struct thread *next;
    struct cpu_regs_on_stack *regs;
};

struct process {
    uint32_t process_id;
    uint8_t regions_en;
    uint8_t padding[3];
    struct mpu_region mem_regions[8]; /* TODO: make this a linked list as well? */
    struct thread threads;
};

#endif /* PROC_MGR_H */


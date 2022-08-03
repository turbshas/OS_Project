#ifndef PROC_MGR_H
#define PROC_MGR_H

#include "cpu.h"
#include "doubly_linked_list.h"
#include "mpu.h"
#include "thread.h"

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
class ProcessManager {
    private:
        DoublyLinkedList<Process *> _processes;
        DoublyLinkedList<Thread *> _readyThreadsRanToCompletion;
        DoublyLinkedList<Thread *> _readyThreadsStoppedEarly;
        DoublyLinkedList<Thread *> _blockedThreads;
        Thread* _runningThreads[NUM_CPUS];

    public:
        ProcessManager();
        ProcessManager(const ProcessManager&) = delete;
        ProcessManager(ProcessManager&&) = delete;
        ~ProcessManager();
        ProcessManager& operator=(const ProcessManager&) = delete;
        ProcessManager& operator=(ProcessManager&&) = delete;

        Thread* CreateThread(Process* parentProcess);
        Thread* ScheduleNextThread(uint32_t core);
};

#endif /* PROC_MGR_H */


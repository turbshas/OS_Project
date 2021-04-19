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

#endif /* PROC_MGR_H */


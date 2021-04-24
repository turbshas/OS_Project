#include "alloc.h"
#include "thread.h"

#define STACK_SIZE (2 * 1024)

static uint32_t threadCounter = 0;
static uint32_t getNextThreadId()
{
    threadCounter++;
    return threadCounter;
}

Thread::Thread(Process &parentProcess)
{
    _threadId = getNextThreadId();
    _parentProcess = &parentProcess;
    _state = ThreadState::Created;
    _next = this;
    _prev = this;
    _privileged = false;
    _useMainStack = true;
    _stack = static_cast<CpuRegsOnStack *>(_ker_malloc(STACK_SIZE));
}

Thread::~Thread()
{
    // These are designed like a circular doubly linked list - no need to check for null
    _next->_prev = _prev;
    _prev->_next = _next;
    // Thread should always have a stack
    _ker_free(STACK_SIZE, _stack);
}

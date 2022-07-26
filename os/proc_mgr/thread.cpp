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
    : _threadId(getNextThreadId()),
    _parentProcess(parentProcess),
    _state(ThreadState::Created),
    _prev(this),
    _next(this), // TODO: placing this thread in parent process list
    _privileged(false),
    _useMainStack(true),
    _stack()
{
}

Thread::Thread(const Thread &source)
    : _threadId(getNextThreadId()),
    _parentProcess(source._parentProcess),
    _state(ThreadState::Created), // TODO: what state to give a copied thread?
    _prev(this), // TODO: is this correct? will proper placement be handled by the caller?
    _next(this),
    _privileged(source._privileged),
    _useMainStack(source._useMainStack),
    _stack(source._stack) // TODO: is this correct?
{
}

Thread::Thread(Thread &&source)
    : _threadId(source._threadId),
    _parentProcess(source._parentProcess),
    _state(source._state),
    _prev(source._prev),
    _next(source._next),
    _privileged(source._privileged),
    _useMainStack(source._useMainStack),
    _stack(source._stack)
{
    source._state = ThreadState::Dead;
    source._prev = &source;
    source._next = &source;
}

Thread::~Thread()
{
    // These are designed like a circular doubly linked list - no need to check for null
    _next->_prev = _prev;
    _prev->_next = _next;
}

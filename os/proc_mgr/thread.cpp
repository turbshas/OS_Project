#include "thread.h"
#include "alloc.h"

#define STACK_SIZE (2 * 1024)

static uint32_t threadCounter = 0;
static uint32_t
getNextThreadId()
{
    threadCounter++;
    return threadCounter;
}

Thread::Thread()
    : _threadId(0),
      _parentProcess(nullptr),
      _state(ThreadState::Dead),
      _privileged(false),
      _useMainStack(true),
      _savedRegs(),
      _stack()
{
}

Thread::Thread(Process& parentProcess, MemoryManager* memMgr)
    : _threadId(getNextThreadId()),
      _parentProcess(&parentProcess),
      _state(ThreadState::Created),
      _privileged(false),
      _useMainStack(true),
      _savedRegs(),
      _stack(memMgr->Allocate(PAGE_SIZE))
{
}

Thread::Thread(const Thread& source)
    : _threadId(source._threadId),
      _parentProcess(source._parentProcess),
      _state(source._state),
      _privileged(source._privileged),
      _useMainStack(source._useMainStack),
      _savedRegs(source._savedRegs),
      _stack(source._stack)
{
}

Thread::Thread(Thread&& source)
    : Thread()
{
    *this = static_cast<Thread&&>(source);
}

Thread::~Thread()
{
}

Thread&
Thread::operator=(const Thread& source)
{
    if (this == &source) return *this;

    _threadId = source._threadId;
    _parentProcess = source._parentProcess;
    _state = source._state;
    _privileged = source._privileged;
    _useMainStack = source._useMainStack;
    _savedRegs = source._savedRegs;
    _stack = source._stack;

    return *this;
}

Thread&
Thread::operator=(Thread&& source)
{
    if (this == &source) return *this;

    _threadId = source._threadId;
    source._threadId = 0;

    _parentProcess = source._parentProcess;
    source._parentProcess = nullptr;

    _state = source._state;
    source._state = ThreadState::Dead;

    _privileged = source._privileged;
    source._privileged = false;

    _useMainStack = source._useMainStack;
    source._useMainStack = true;

    _savedRegs = source._savedRegs;
    // No need to destruct CPU regs - only stores data.

    _stack = source._stack;

    return *this;
}

KernelResultStatus
Thread::SetEntryPoint(const VoidFunction startAddress)
{
    if (startAddress == nullptr || _state != ThreadState::Created) return KernelResultStatus::Error;
    auto stackedRegs = GetStackedRegisters();
    stackedRegs->PC = reinterpret_cast<uint32_t>(startAddress);
    return KernelResultStatus::Success;
}

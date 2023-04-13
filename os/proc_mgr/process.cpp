#include "process.h"

#define ROOT_PROCESS_ID 1

static uint32_t processCounter = 1;
static uint32_t
getNextProcessId()
{
    processCounter++;
    return processCounter;
}

Process::Process(const uint32_t parentProcessId, MemoryManager* const memMgr)
    : _parentProcessId(0),
      _processId(ROOT_PROCESS_ID),
      _memMgr(memMgr),
      _state(ProcessState::Created),
      _swapped(false),
      _returnCode(0),
      _memRegionList(),
      _threadList()
{
    const MemRegion initialMemRegion = _memMgr->Allocate(PAGE_SIZE);
    _memRegionList.pushBack(initialMemRegion);
    const Thread initialThread{*this};
    _threadList.pushBack(initialThread);
}

Process::~Process()
{
    // Empty thread list and call destructor on each thread since threads are stored by value so won't get called otherwise.
    // I think this is fine since it's a copy of the original?
    while (!_threadList.empty())
    {
        Thread thread = _threadList.popFront();
        thread.~Thread();
    }
    while (_memRegionList.empty())
    {
        const MemRegion memRegion = _memRegionList.popFront();
        _memMgr->Free(memRegion);
    }
}

void
Process::AddMemRegion(const MemRegion& memRegion)
{
    _memRegionList.pushBack(memRegion);
}

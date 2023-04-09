#include "process.h"

#define ROOT_PROCESS_ID 1

static uint32_t processCounter = 1;
static uint32_t
getNextProcessId()
{
    processCounter++;
    return processCounter;
}

Process::Process()
    : _parentProcessId(ROOT_PROCESS_ID),
      _processId(getNextProcessId()),
      _state(ProcessState::Created),
      _swapped(false),
      _returnCode(0),
      _memRegionList(),
      _threadList()
{
    _threadList.pushFront(new Thread(*this));
}

Process::~Process()
{
    while (!_threadList.empty())
    {
        Thread* thread = _threadList.popFront();
        delete thread;
    }
}

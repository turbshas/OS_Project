#include "process.h"

#define ROOT_PROCESS_ID 1

static uint32_t processCounter = 0;
static uint32_t getNextProcessId()
{
    processCounter++;
    return processCounter;
}

Process::Process()
{
    _parentProcessId = ROOT_PROCESS_ID;
    _processId = getNextProcessId();
    _state = ProcessState::Created;
    _swapped = false;
    _returnCode = 0;
    _activeRegions = 0x1;
    _numThreads = 1;
    _firstThread = new Thread(this);
    _lastThread = _firstThread;
}

Process::~Process()
{
    while (_firstThread != _lastThread) {
        delete _firstThread;
    }
}

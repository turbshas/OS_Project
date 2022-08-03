#include "proc_mgr.h"

ProcessManager::ProcessManager()
    : _processes(),
    _readyThreadsRanToCompletion(),
    _readyThreadsStoppedEarly(),
    _blockedThreads(),
    _runningThreads()
{
}

ProcessManager::~ProcessManager()
{
}

Thread* ProcessManager::CreateThread(Process* parentProcess)
{
}

Thread* ProcessManager::ScheduleNextThread(uint32_t core)
{
}

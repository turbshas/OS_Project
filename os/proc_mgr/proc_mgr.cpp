#include "proc_mgr.h"

ProcessManager::ProcessManager()
    : _ranToCompletionThreadQueue(),
    _blockedThreadQueue(),
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

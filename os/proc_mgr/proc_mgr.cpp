#include "proc_mgr.h"

ProcessManager processManager;

ProcessManager::ProcessManager()
    : _memMgr(nullptr),
      _kernelProcess(0, nullptr),
      _processes(),
      _readyThreadsRanToCompletion(),
      _readyThreadsStoppedEarly(),
      _blockedThreads(),
      _runningThreads()
{
}

ProcessManager::~ProcessManager()
{
    // Intentionally do nothing.
}

void
ProcessManager::Initialize(const MemoryManager& memMgr)
{
    _memMgr = &memMgr;
}

Thread*
ProcessManager::CreateThread(Process* parentProcess)
{
}

Thread*
ProcessManager::ScheduleNextThread(uint32_t core)
{
}

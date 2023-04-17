#include "proc_mgr.h"

ProcessManager processManager;

ProcessManager::ProcessManager()
    : _memMgr(nullptr),
      _kernelProcess(0, nullptr, nullptr),
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
ProcessManager::Initialize(MemoryManager& memMgr, const KernelApi& kernelApi)
{
    _memMgr = &memMgr;
    _kernelProcess = Process{0, _memMgr, kernelApi.ApiEntry};
}

Thread*
ProcessManager::CreateThread(Process* parentProcess)
{
}

Thread*
ProcessManager::ScheduleNextThread(uint32_t core)
{
}

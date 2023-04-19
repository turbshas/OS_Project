#include "proc_mgr.h"

ProcessManager processManager;

ProcessManager::ProcessManager()
    : _memMgr(nullptr),
      _kernelProcess(), // Don't pass in nullptr - it will try to allocate stack for a thread!
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
    const auto kernelThread = _kernelProcess.GetMainThread();
    kernelThread->SetThreadMode(true, false);
}

Process*
ProcessManager::CreateProcess(const VoidFunction start)
{
    auto process = new Process(ROOT_PROCESS_ID, _memMgr, start);
    _processes.pushBack(process);
    return process;
}

Thread*
ProcessManager::CreateThread(Process* parentProcess)
{
}

Thread*
ProcessManager::ScheduleNextThread(uint32_t core)
{
}

#include "kernel_api.hpp"
#include "proc_mgr.h"

static void
ApiEntryFunction()
{
    auto kernelProcess = processManager.GetKernelProcess();
    auto kernelThread = kernelProcess->GetMainThread();
    auto stackedRegisters = kernelThread->GetStackedRegisters();
    auto savedRegisters = kernelThread->getSavedRegisters();
}

namespace os::api
{
    KernelApi kernelApi;

    KernelApi::KernelApi()
        : ApiEntry(ApiEntryFunction)
    {
    }

    void
    KernelApi::ProcessRequest(const ApiRequest& request)
    {
    }
}

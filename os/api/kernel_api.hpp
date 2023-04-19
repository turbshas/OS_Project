#ifndef _KERNEL_API_H
#define _KERNEL_API_H

#include "api_request.hpp"
#include "misc.hpp"
#include "savedRegisters.hpp"

namespace os::api
{
    class KernelApi
    {
        public:
            VoidFunction ApiEntry;

            KernelApi();
            void ProcessRequest(const ApiRequest& request);
    };

    extern KernelApi kernelApi;
}

#endif /* _KERNEL_API_H */

#ifndef _API_REQUEST_H
#define _API_REQUEST_H

#include <cstdint>

namespace os::api
{
    class ApiRequest
    {
        private:
            const uint32_t _param0;
            const uint32_t _param1;
            const uint32_t _param2;

        public:
            ApiRequest(const uint32_t param0, const uint32_t param1, const uint32_t param2);

            uint32_t GetParam0() const { return _param0; };
            uint32_t GetParam1() const { return _param1; };
            uint32_t GetParam2() const { return _param2; };
    };
}

#endif /* _API_REQUEST_H */

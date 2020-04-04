#ifndef MPU_H
#define MPU_H

#include <cstdio>

#include "mpu_region.h"

class Mpu {
    uint32_t TYPE;
    uint32_t CTRL;
    uint32_t RNR;
    uint32_t RBAR;
    uint32_t RASR;
    uint32_t RBAR_A1;
    uint32_t RASR_A1;
    uint32_t RBAR_A2;
    uint32_t RASR_A2;
    uint32_t RBAR_A3;
    uint32_t RASR_A3;

    private:
        uint32_t get_rbar(const unsigned num, const mpu_region &region) const volatile;
        uint32_t get_rasr(const mpu_region &region) const volatile;

    public:
        void init(void) volatile;
        int get_config(const unsigned num, mpu_region &region) volatile;
        int set_config(const unsigned num, const mpu_region &region) volatile;
        int region_enable(const unsigned num) volatile;
        int region_disable(const unsigned num) volatile;
};

extern volatile Mpu *const MPU;

/* Notes:
 *  - Enable MPU
 *  - Disable MPU during faults
 *  - Enable default region
 */
void mpu_init(void);

#endif /* MPU_H */


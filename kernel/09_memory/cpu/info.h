#ifndef INFO_H_
#define INFO_H_

#include "types.h"

typedef struct CpuidRegisters {
    uint32_t eax, ebx, ecx, edx;
} cpuid_registers;

enum ProcessorType { ORIGINAL_OEM, INTEL_OVERDRIVE, DUAL_PROCESSOR, INTEL_RESERVED };

typedef struct CpuInformation {
    // eax
    /* uint8_t stepping_id; */
    uint8_t model;
    uint16_t family_id;
    enum ProcessorType processor_type;

    // ebx
    /* uint8_t brand_index; */
    uint16_t clflush_line_size;
    uint8_t maximum_logical_processors;
    uint8_t initial_apic_id;

    // ecx

    // edx
    bool apic;
    bool sep;
    bool mmx;
} cpu_information;

cpuid_registers cpuid(uint32_t eax, uint32_t ecx);
cpu_information cpu_info();

#endif // INFO_H_

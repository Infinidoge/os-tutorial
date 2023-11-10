#include "info.h"

cpuid_registers cpuid(uint32_t input_eax, uint32_t input_ecx) {
    uint32_t eax, ebx, ecx, edx;
    asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(input_eax), "c"(input_ecx));

    cpuid_registers registers = {eax, ebx, ecx, edx};
    return registers;
}

cpu_information cpu_info() {
    cpuid_registers registers = cpuid(1, 0);

    uint16_t family_id = NIBBLE(registers.eax, 8);
    uint8_t model = NIBBLE(registers.eax, 4);

    if (family_id == 0x6 || family_id == 0xF)
        model = (registers.eax >> 12 & 0xF0) | model;

    if (family_id == 0xF)
        family_id += BYTE(registers.eax, 20);

    enum ProcessorType type;
    switch (registers.eax >> 12 & 0b11) {
    case 0b00:
        type = ORIGINAL_OEM;
        break;
    case 0b01:
        type = INTEL_OVERDRIVE;
        break;
    case 0b10:
        type = DUAL_PROCESSOR;
        break;
    case 0b11:
        type = INTEL_RESERVED;
        break;
    }

    bool htt = BIT(registers.edx, 28);
    uint16_t logical_processors = 1;

    if (htt) {
        uint8_t logical_processors_raw = BYTE(registers.ebx, 16);
        while (logical_processors < logical_processors_raw)
            logical_processors <<= 1;
    }

    cpu_information information = {
        /* .stepping_id = NIBBLE(registers.eax, 0), */
        .model = model,
        .family_id = family_id,
        .processor_type = type,

        /* .brand_index = BYTE(registers.ebx, 0), */
        .clflush_line_size = BYTE(registers.ebx, 8) * 8,
        .maximum_logical_processors = htt ? logical_processors : 1,
        .initial_apic_id = BYTE(registers.ebx, 24),

        .apic = BIT(registers.edx, 9),
        .sep = BIT(registers.edx, 11),
        .mmx = BIT(registers.edx, 23),
    };

    return information;
}

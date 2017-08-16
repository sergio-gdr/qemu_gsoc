/*
 * QEMU Hypervisor.framework (HVF) support
 *
 * Copyright Google Inc., 2017
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 */

/* header to be included in non-HVF-specific code */
#ifndef _HVF_H
#define _HVF_H

#include "config-host.h"
#include "qemu/osdep.h"
#include "qemu-common.h"
#include "qemu/bitops.h"
#include "exec/memory.h"
#include "sysemu/accel.h"
#include "target/i386/hvf-utils/x86_gen.h"

extern int hvf_disabled;
#ifdef CONFIG_HVF
#include <Hypervisor/hv.h>
#include <Hypervisor/hv_vmx.h>
#include <Hypervisor/hv_error.h>
#include "hw/hw.h"
uint32_t hvf_get_supported_cpuid(uint32_t func, uint32_t idx,
                                 int reg);
#define hvf_enabled() !hvf_disabled
#else
#define hvf_enabled() 0
#define hvf_get_supported_cpuid(func, idx, reg) 0
#endif

typedef struct hvf_slot {
    uint64_t start;
    uint64_t size;
    uint8_t *mem;
    int slot_id;
} hvf_slot;

typedef struct hvf_vcpu_caps {
    uint64_t vmx_cap_pinbased;
    uint64_t vmx_cap_procbased;
    uint64_t vmx_cap_procbased2;
    uint64_t vmx_cap_entry;
    uint64_t vmx_cap_exit;
    uint64_t vmx_cap_preemption_timer;
} hvf_vcpu_caps;

typedef struct HVFState {
    AccelState parent;
    hvf_slot slots[32];
    int num_slots;

    hvf_vcpu_caps *hvf_caps;
} HVFState;
extern HVFState *hvf_state;

typedef struct HVFX86EmulatorState {
    int interruptable;
    uint64_t fetch_rip;
    uint64_t rip;
    struct x86_register {
        union {
            struct {
                uint64_t rrx; /* full 64 bit */
            };
            struct {
                uint32_t erx; /* low 32 bit part */
                uint32_t hi32_unused1;
            };
            struct {
                uint16_t rx; /* low 16 bit part */
                uint16_t hi16_unused1;
                uint32_t hi32_unused2;
            };
            struct {
                uint8_t lx; /* low 8 bit part */
                uint8_t hx; /* high 8 bit */
                uint16_t hi16_unused2;
                uint32_t hi32_unused3;
            };
        };
    } __attribute__ ((__packed__)) regs[16];
    /* rflags register */
    struct x86_reg_flags {
        union {
            struct {
                uint64_t rflags;
            };
            struct {
                uint32_t eflags;
                uint32_t hi32_unused1;
            };
            struct {
                uint32_t cf:1;
                uint32_t unused1:1;
                uint32_t pf:1;
                uint32_t unused2:1;
                uint32_t af:1;
                uint32_t unused3:1;
                uint32_t zf:1;
                uint32_t sf:1;
                uint32_t tf:1;
                uint32_t ief:1;
                uint32_t df:1;
                uint32_t of:1;
                uint32_t iopl:2;
                uint32_t nt:1;
                uint32_t unused4:1;
                uint32_t rf:1;
                uint32_t vm:1;
                uint32_t ac:1;
                uint32_t vif:1;
                uint32_t vip:1;
                uint32_t id:1;
                uint32_t unused5:10;
                uint32_t hi32_unused2;
            };
        };
    } __attribute__ ((__packed__)) rflags;
    struct lazy_flags {
        addr_t result;
        addr_t auxbits;
    } lflags;
    struct x86_efer {
        uint64_t efer;
    } __attribute__ ((__packed__)) efer;
    uint8_t mmio_buf[4096];
} HVFX86EmulatorState;
#ifdef CONFIG_HVF
#include "target/i386/cpu.h"
#endif

void hvf_set_phys_mem(MemoryRegionSection *, bool);
hvf_slot *hvf_find_overlap_slot(uint64_t, uint64_t);

/* Disable HVF if |disable| is 1, otherwise, enable it iff it is supported by
 * the host CPU. Use hvf_enabled() after this to get the result. */
void hvf_disable(int disable);

/* Returns non-0 if the host CPU supports the VMX "unrestricted guest" feature
 * which allows the virtual CPU to directly run in "real mode". If true, this
 * allows QEMU to run several vCPU threads in parallel (see cpus.c). Otherwise,
 * only a a single TCG thread can run, and it will call HVF to run the current
 * instructions, except in case of "real mode" (paging disabled, typically at
 * boot time), or MMIO operations. */

int hvf_sync_vcpus(void);

int hvf_init_vcpu(CPUState *);
int hvf_vcpu_exec(CPUState *);
int hvf_smp_cpu_exec(CPUState *);
void hvf_cpu_synchronize_state(CPUState *);
void hvf_cpu_synchronize_post_reset(CPUState *);
void hvf_cpu_synchronize_post_init(CPUState *);
void _hvf_cpu_synchronize_post_init(CPUState *, run_on_cpu_data);

void hvf_vcpu_destroy(CPUState *);
void hvf_raise_event(CPUState *);
/* void hvf_reset_vcpu_state(void *opaque); */
void hvf_reset_vcpu(CPUState *);
void vmx_update_tpr(CPUState *);
void update_apic_tpr(CPUState *);
int hvf_put_registers(CPUState *);
void vmx_clear_int_window_exiting(CPUState *cpu);

#define TYPE_HVF_ACCEL ACCEL_CLASS_NAME("hvf")

#define HVF_STATE(obj) \
    OBJECT_CHECK(HVFState, (obj), TYPE_HVF_ACCEL)

#endif

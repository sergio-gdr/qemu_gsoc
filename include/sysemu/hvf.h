/*
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 */

#ifndef QEMU_HVF_INT_H
#define QEMU_HVF_INT_H

#include "sysemu/sysemu.h"
#include "sysemu/accel.h"
#include <Hypervisor/hv.h>
#include <Hypervisor/hv_error.h>
#include <Hypervisor/hv_vmx.h>

typedef struct HVFSlot {
    hwaddr start_addr;
    ram_addr_t size;
    void *ram;
    int slot_id;
    int flags;
} HVFSlot;

typedef struct {
    AccelState parent_obj;
    HVFSlot slots[32];
    int num_slots;
} HVFState;

//typedef struct KVMMemoryListener {
//    MemoryListener listener;
//    KVMSlot *slots;
//    int as_id;
//} KVMMemoryListener0;

#define TYPE_HVF_ACCEL ACCEL_CLASS_NAME("hvf")
#define HVF_STATE(obj) \
    OBJECT_CHECK(HVFState, (obj), TYPE_HVF_ACCEL)

//void kvm_memory_listener_register(KVMState *s, KVMMemoryListener *kml,
                   //               AddressSpace *as, int as_id);

#endif

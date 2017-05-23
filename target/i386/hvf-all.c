#include "qemu/osdep.h"
#include "qemu-common.h"
#include "qom/object.h"
#include "hw/boards.h"
#include "sysemu/hvf.h"

HVFState *hvf_state;

static int print_err(hv_return_t err)
{
 switch (err) {
  case HV_ERROR:
   printf("Error: HV_ERROR\n");
   break;
  case HV_BUSY:
   printf("Error: HV_BUSY\n");
   break;
  case HV_BAD_ARGUMENT:
   printf("Error: HV_BAD_ARGUMENT\n");
   break;
  case HV_NO_RESOURCES:
   printf("Error: HV_NO_RESOURCES\n");
   break;
  case HV_NO_DEVICE:
   printf("Error: HV_NO_DEVICE\n");
   break;
  case HV_UNSUPPORTED:
   printf("Error: HV_UNSUPPORTED\n");
   break;
  default:
   printf("Unknown Error\n");
 }
 return -1; // XXX
}

static bool hvf_allowed;

static int hvf_accel_init(MachineState *ms)
{
    int x, err;
    hv_return_t ret;
    HVFState *s;

    if ( (ret = hv_vm_create(HV_VM_DEFAULT)) != HV_SUCCESS) {
        err = print_err(ret);
        return err;
    }

    s = HVF_STATE(ms->accelerator);

    s->num_slots = 32;
    for (x = 0; x < s->num_slots; ++x) {
        s->slots[x].size = 0;
        s->slots[x].slot_id = x;
    }

    hvf_state = s;
    //cpu_interrupt_handler = hvf_handle_interrupt;
   //memory_listener_register(&hvf_memory_listener, &address_space_memory);
   //memory_listener_register(&hvf_io_listener, &address_space_io);
    return 0;
}

static void hvf_accel_class_init(ObjectClass *oc, void *data)
{
    AccelClass *ac = ACCEL_CLASS(oc);
    ac->name = "HVF";
    ac->init_machine = hvf_accel_init;
    ac->allowed = &hvf_allowed;
}

static const TypeInfo hvf_accel_type = {
    .name = ACCEL_CLASS_NAME("hvf"),
    .parent = TYPE_ACCEL,
    .class_init = hvf_accel_class_init,
};

static void hvf_type_init(void)
{
    type_register_static(&hvf_accel_type);
}

type_init(hvf_type_init);

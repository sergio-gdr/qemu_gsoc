/*
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */
void x86_cpu_xsave_all_areas(X86CPU *cpu, void *buf);
void x86_cpu_xrstor_all_areas(X86CPU *cpu, void *buf);

/* for KVM_CAP_XSAVE */
struct xsave_buf {
	uint32_t region[1024];
};


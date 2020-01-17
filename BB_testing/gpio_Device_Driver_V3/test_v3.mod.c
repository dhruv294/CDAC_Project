#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x2cce4d13, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xd36ea1cf, __VMLINUX_SYMBOL_STR(param_ops_uint) },
	{ 0x706d051c, __VMLINUX_SYMBOL_STR(del_timer_sync) },
	{ 0x6921701b, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xc3873942, __VMLINUX_SYMBOL_STR(class_remove_file_ns) },
	{ 0x93e0f114, __VMLINUX_SYMBOL_STR(class_create_file_ns) },
	{ 0xd40092b8, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x1bb31047, __VMLINUX_SYMBOL_STR(add_timer) },
	{ 0x9580deb, __VMLINUX_SYMBOL_STR(init_timer_key) },
	{ 0xe9d56913, __VMLINUX_SYMBOL_STR(device_create_file) },
	{ 0x2064a460, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0xc43a9abd, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xa3c94160, __VMLINUX_SYMBOL_STR(device_remove_file) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0xe2861e64, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0x20c55ae0, __VMLINUX_SYMBOL_STR(sscanf) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x11a13e31, __VMLINUX_SYMBOL_STR(_kstrtol) },
	{ 0x8f678b07, __VMLINUX_SYMBOL_STR(__stack_chk_guard) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x16e5c2a, __VMLINUX_SYMBOL_STR(mod_timer) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0xe38ecd52, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0x257e3b55, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "6890F3F07EC5BB4F75E77CF");

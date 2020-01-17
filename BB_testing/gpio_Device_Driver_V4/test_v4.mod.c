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
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0x6921701b, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0xf3362c4f, __VMLINUX_SYMBOL_STR(gpiod_to_irq) },
	{ 0xc43a9abd, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x2064a460, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xd40092b8, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0x823c273a, __VMLINUX_SYMBOL_STR(gpiod_set_debounce) },
	{ 0x73c1fe20, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0xe38ecd52, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x235992ba, __VMLINUX_SYMBOL_STR(gpiod_get_raw_value) },
	{ 0xe2861e64, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0x257e3b55, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "53C7E3C5F4496A7BC5C652B");

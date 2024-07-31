#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0x8d7e33a7, "module_layout" },
	{ 0x3fee8a5c, "param_ops_uint" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x680075ef, "dma_release_channel" },
	{ 0x37a0cba, "kfree" },
	{ 0x449ad0a7, "memcmp" },
	{ 0xe76c6ad3, "dma_sync_wait" },
	{ 0xb43f9365, "ktime_get" },
	{ 0xc5850110, "printk" },
	{ 0x8554887e, "__dma_request_channel" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0x2d6d8d3a, "dma_direct_map_page" },
	{ 0x97651e6c, "vmemmap_base" },
	{ 0x4c9d28b0, "phys_base" },
	{ 0x56470118, "__warn_printk" },
	{ 0xbd1dea0, "dev_driver_string" },
	{ 0x9cb986f2, "vmalloc_base" },
	{ 0x2ea2c95c, "__x86_indirect_thunk_rax" },
	{ 0x12a9f2f4, "dma_direct_unmap_page" },
	{ 0x713b0fa0, "dma_ops" },
	{ 0xeb233a45, "__kmalloc" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "1351553DB1DD7D5004D918A");

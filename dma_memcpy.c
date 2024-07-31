#include <linux/module.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/dmaengine.h>
#include <linux/async_tx.h>
#include <linux/platform_device.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/string.h> // for memcpy

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("DMA and libc memcpy Comparison with Verification");

static unsigned int copy_size = (1 * 1024 * 1024); // 默认1 MB
module_param(copy_size, uint, 0644);
MODULE_PARM_DESC(copy_size, "Size of the memory copy operation in bytes");

static int __init dma_memcpy_init(void)
{
    struct dma_chan *chan;
    struct dma_device *dma_dev;
    dma_addr_t src_dma, dst_dma;
    void *src, *dst;
    void *src_memcpy, *dst_memcpy;
    struct dma_async_tx_descriptor *tx;
    dma_cookie_t cookie;
    enum dma_ctrl_flags flags = DMA_CTRL_ACK | DMA_PREP_INTERRUPT;
    dma_cap_mask_t mask;
    ktime_t start, end;
    s64 duration_dma, duration_memcpy;
    int ret = 0;

    // 初始化DMA能力掩码
    dma_cap_zero(mask);
    dma_cap_set(DMA_MEMCPY, mask);

    // 请求一个DMA通道
    chan = dma_request_channel(mask, NULL, NULL);
    if (!chan) {
        pr_err("Failed to request DMA channel\n");
        return -ENODEV;
    }

    dma_dev = chan->device;

    // 分配源和目的内存
    src = kzalloc(copy_size, GFP_KERNEL);
    if (!src) {
        ret = -ENOMEM;
        goto free_chan;
    }

    dst = kzalloc(copy_size, GFP_KERNEL);
    if (!dst) {
        ret = -ENOMEM;
        goto free_src;
    }

    // 初始化源内存
    memset(src, 0xaa, copy_size);

    // 映射内存到DMA地址空间
    src_dma = dma_map_single(dma_dev->dev, src, copy_size, DMA_TO_DEVICE);
    if (dma_mapping_error(dma_dev->dev, src_dma)) {
        pr_err("Failed to map source memory\n");
        ret = -EIO;
        goto free_dst;
    }

    dst_dma = dma_map_single(dma_dev->dev, dst, copy_size, DMA_FROM_DEVICE);
    if (dma_mapping_error(dma_dev->dev, dst_dma)) {
        pr_err("Failed to map destination memory\n");
        ret = -EIO;
        goto unmap_src;
    }

    // 准备DMA传输
    tx = dma_dev->device_prep_dma_memcpy(chan, dst_dma, src_dma, copy_size, flags);
    if (!tx) {
        pr_err("Failed to prepare DMA memcpy\n");
        ret = -EIO;
        goto unmap_dst;
    }

    // 获取开始时间戳
    start = ktime_get();

    // 提交传输
    cookie = tx->tx_submit(tx);
    if (dma_submit_error(cookie)) {
        pr_err("Failed to do DMA tx_submit\n");
        ret = -EIO;
        goto unmap_dst;
    }

    // 等待传输完成
    dma_async_issue_pending(chan);
    dma_sync_wait(chan, cookie);

    // 获取结束时间戳
    end = ktime_get();

    // 计算用时
    duration_dma = ktime_to_ns(ktime_sub(end, start));
    pr_info("DMA Memory Copy of %u bytes completed successfully in %lld ns\n", copy_size, duration_dma);

    // 验证DMA复制的正确性
    if (memcmp(src, dst, copy_size) == 0) {
        pr_info("DMA Memory Copy verified successfully\n");
    } else {
        pr_err("DMA Memory Copy verification failed\n");
    }

    // 使用libc memcpy进行内存复制
    src_memcpy = kmalloc(copy_size, GFP_KERNEL);
    if (!src_memcpy) {
        ret = -ENOMEM;
        goto unmap_dst;
    }

    dst_memcpy = kmalloc(copy_size, GFP_KERNEL);
    if (!dst_memcpy) {
        ret = -ENOMEM;
        goto free_src_memcpy;
    }

    memset(src_memcpy, 0xaa, copy_size); // 初始化源内存

    // 获取开始时间戳
    start = ktime_get();

    // 执行内存复制
    memcpy(dst_memcpy, src_memcpy, copy_size);

    // 获取结束时间戳
    end = ktime_get();

    // 计算用时
    duration_memcpy = ktime_to_ns(ktime_sub(end, start));
    pr_info("libc memcpy of %u bytes completed successfully in %lld ns\n", copy_size, duration_memcpy);

    // 验证libc memcpy复制的正确性
    if (memcmp(src_memcpy, dst_memcpy, copy_size) == 0) {
        pr_info("libc memcpy verified successfully\n");
    } else {
        pr_err("libc memcpy verification failed\n");
    }

    kfree(dst_memcpy);
free_src_memcpy:
    kfree(src_memcpy);
unmap_dst:
    dma_unmap_single(dma_dev->dev, dst_dma, copy_size, DMA_FROM_DEVICE);
unmap_src:
    dma_unmap_single(dma_dev->dev, src_dma, copy_size, DMA_TO_DEVICE);
free_dst:
    kfree(dst);
free_src:
    kfree(src);
free_chan:
    dma_release_channel(chan);

    return ret;
}

static void __exit dma_memcpy_exit(void)
{
    pr_info("DMA and libc memcpy Comparison module exited\n");
}

module_init(dma_memcpy_init);
module_exit(dma_memcpy_exit);
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>

#define DEVICE_NAME	"myserial"
#define DRIVER_NAME	"uart_dummy"

#define MY_UART_BASE 0x3f8
#define MY_UART_END  0x3f8
#define MY_UART_IRQ  4

struct myserial_dev {
	struct platform_device *pdev;
	struct miscdevice miscdev;
	int irq;
	void __iomem *regs;
};
struct resource my_uart_res[] = {
	[0] = {
		.start  =  MY_UART_BASE,
		.end    =  MY_UART_END,
		.flags  =  IORESOURCE_IO,
	},
	[1] = {
		.start  = MY_UART_IRQ ,
		.end    = MY_UART_IRQ,//same as start,as only one IRQ line
		.flags  = IORESOURCE_IRQ,
	},
};

struct platform_device myuart_device = {
	.name           = DRIVER_NAME,
	.id             = -1,
	.resource       = my_uart_res,
	.num_resources  = ARRAY_SIZE(my_uart_res),
};

int __init myserial_dev_init(void)
{
	int ret;
	ret=platform_device_register(&myuart_device);
	if(ret)
		printk("error in registering platform device\n");
    printk("successfully registered dummy platform device\n");
	return 0;

}
void __exit myserial_dev_exit(void)
{
    platform_device_unregister(&myuart_device);
	printk("platform device unregisted\n");
}

module_init(myserial_dev_init);
module_exit(myserial_dev_exit);

MODULE_LICENSE("GPL");

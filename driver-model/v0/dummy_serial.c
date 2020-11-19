#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#define DEVICE_NAME	"uart_dummy"
#define DRIVER_NAME	"uart_dummy"

#define MY_UART_BASE 0x9000
#define MY_UART_END  0x9020
#define MY_UART_IRQ  130

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

static int myserial_probe(struct platform_device *pfdev)
{
  printk("Dummy serial driver -- probe method\n");
  return 0;
}
static int myserial_remove(struct platform_device *pdev)
{
   printk("Dummy serial driver -- remove method\n");
   return 0;
}
struct platform_device myuart_device = {
	.name           = DEVICE_NAME,
	.id             = -1,
	.resource       = my_uart_res,
	.num_resources  = ARRAY_SIZE(my_uart_res),
};
static struct platform_driver myserial_driver={
	.probe		= myserial_probe,
	.remove		= myserial_remove,
	.driver		= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
	},
};
int __init myserial_init(void)
{
	int ret;
	ret=platform_device_register(&myuart_device);
	if(ret)
		printk("error in registering platform device\n");
	ret=platform_driver_register(&myserial_driver);
	if(ret)
		printk("error in registering myserial driver\n");	
    printk("successfully registered dummy serial driver");
	return 0;

}
void __exit myserial_exit(void)
{
	platform_driver_unregister(&myserial_driver);
	printk("dummy serial driver unregistered successfully\n");	
    platform_device_unregister(&myuart_device);
	printk("platform device unregisted\n");
}

module_init(myserial_init);
module_exit(myserial_exit);

MODULE_LICENSE("GPL");

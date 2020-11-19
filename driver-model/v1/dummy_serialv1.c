#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>

#define DEVICE_NAME	"myserial"
#define DRIVER_NAME	"uart_dummy"

#define MY_UART_BASE 0x9000
#define MY_UART_END  0x9000
#define MY_UART_IRQ  5

/*static const struct of_device_id fes_of_ids[] = {
	{ .compatible = "free-electrons,serial" },
	{}
};*/
struct myserial_dev {
	struct platform_device *pdev;
	struct miscdevice miscdev;
	int irq;
	void __iomem *regs;
};
static int myserial_open(struct inode *i, struct file *f)
{
    printk("myserial -- open\n");
	return 0;
}
static int myserial_close(struct inode *i, struct file *f)
{
    printk("myserial -- close\n");
	return 0;
}
static long myserial_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    printk("myserial -- ioctl\n");
	return 0;
}
static ssize_t myserial_write(struct file *f, const char __user *ubuf,
	size_t size, loff_t *off)
{
    printk("myserial -- write\n");
	return 0;
}
static ssize_t myserial_read(struct file *f, char __user *ubuf, size_t size,
	loff_t *off)
{
    printk("myserial -- read\n");
	return 0;
}
static const struct file_operations myserial_fops = {
	.read = myserial_read,
	.write = myserial_write,
	.unlocked_ioctl = myserial_ioctl,
	.open = myserial_open,
    .release = myserial_close
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

static int myserial_probe(struct platform_device *pdev)
{
  int ret;
  struct resource *res;
  struct myserial_dev *dev;
  unsigned int baud_divisor, uartclk;
  pr_info("Called feserial_probe\n");
  res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
  if (res == NULL) {
	pr_err("Unable to obtain platform memory resource\n");
	return -1;
  }
  dev = devm_kzalloc(&pdev->dev, sizeof(struct myserial_dev), GFP_KERNEL);
  if (!dev) {
	pr_err("Unable to obtain kernel memory\n");
	return -ENOMEM;
  }
  dev->irq = platform_get_irq(pdev, 0);

  /*dev->regs = devm_ioremap_resource(&pdev->dev, res);    //request_region
  if (!dev->regs) {
	dev_err(&pdev->dev, "Cannot remap registers\n");
	return -ENOMEM;
  }*/
  /* ret = devm_request_irq(dev->irq, my_serial_int_handler, IRQF_SHARED,
                                                "dummy_serial", &dev);
  */
  dev->miscdev.minor = MISC_DYNAMIC_MINOR;
  dev->miscdev.name = kasprintf(GFP_KERNEL, "myserial-%x",res->start);
  dev->miscdev.fops = &myserial_fops;
  platform_set_drvdata(pdev, dev);
  ret = misc_register(&dev->miscdev);
  if (ret) {
	pr_err("Failed to register misc console\n");
	return -ENODEV;
  }
  pr_info("dummy serial: uart base is : %x\n", res->start);
  pr_info("dummy serial: irq number is : %d\n", dev->irq);
  printk("Dummy serial driver -- probe method\n");
  return 0;
}
static int myserial_remove(struct platform_device *pdev)
{
   printk("Dummy serial driver -- remove method\n");
   return 0;
}
struct platform_device myuart_device = {
	.name           = DRIVER_NAME,
	.id             = -1,
	.resource       = my_uart_res,
	.num_resources  = ARRAY_SIZE(my_uart_res),
};
static struct platform_driver myserial_driver={
	.probe		=myserial_probe,
	.remove		=myserial_remove,
	.driver		={
		.name	=DRIVER_NAME,
		.owner	=THIS_MODULE,
	},
};
int __init myserial_init(void)
{
	int ret;
	ret=platform_device_register(&myuart_device);
	if(ret)
		printk("error in registering platform device\n");
    printk("successfully registered dummy platform device\n");
	ret=platform_driver_register(&myserial_driver);
	if(ret)
		printk("error in registering myserial driver\n");	
    printk("successfully registered dummy serial driver\n");
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>

#define DEVICE_NAME	"myserial"
#define DRIVER_NAME	"uart_dummy"

struct myserial_dev {
	struct platform_device *pdev;
	struct miscdevice miscdev;
	int irq;
	void __iomem *regs;
};
static int myserial_open(struct inode *i, struct file *f)
{
	return 0;
}
static long myserial_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	return 0;
}
static ssize_t myserial_write(struct file *f, const char __user *ubuf,
	size_t size, loff_t *off)
{
	return 0;
}
static ssize_t myserial_read(struct file *f, char __user *ubuf, size_t size,
	loff_t *off)
{
	return 0;
}
static const struct file_operations myserial_fops = {
	.read = myserial_read,
	.write = myserial_write,
	.unlocked_ioctl = myserial_ioctl,
	.open = myserial_open,
};

static int myserial_probe(struct platform_device *pdev)
{
  int ret;
  struct resource *res;
  struct myserial_dev *dev;
  unsigned int baud_divisor=0, uartclk=0;
  pr_info("Called myserial_probe\n");
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
  /*dev->regs = devm_ioremap_resource(&pdev->dev, res);
  if (!dev->regs) {
	dev_err(&pdev->dev, "Cannot remap registers\n");
	return -ENOMEM;
  }*/
  dev->miscdev.minor = MISC_DYNAMIC_MINOR;
  dev->miscdev.name = kasprintf(GFP_KERNEL, "myserial-%x",res->start);
  dev->miscdev.fops = &myserial_fops;
  platform_set_drvdata(pdev, dev);
  //dev = (struct feserial_dev *) platform_get_drvdata(pdev);
  //of_property_read_u32(pdev->dev.of_node, "clock-frequency", &uartclk);
  //baud_divisor = uartclk / 16 / 115200;
  //TODO: reg write for baudrate setting
  ret = misc_register(&dev->miscdev);
  if (ret) {
	pr_err("Failed to register misc console\n");
	return -ENODEV;
  }
  pr_info("dummy serial: uart base is : %x\n", res->start);
  pr_info("dummy serial: irq number is : %d\n", dev->irq);
  pr_info("dummy serial: clock frequency is : %x\n", uartclk);
  pr_info("dummy serial: baud divisor is : %d\n", baud_divisor);
  printk("Dummy serial driver -- probe method\n");
  return 0;
}
static int myserial_remove(struct platform_device *pdev)
{
   printk("Dummy serial driver -- remove method\n");
   return 0;
}
static struct platform_driver myserial_driver={
	.probe		=myserial_probe,
	.remove		=myserial_remove,
	.driver		={
		.name	=DRIVER_NAME,
		.owner	=THIS_MODULE,
	},
};
int __init myserial_drv_init(void)
{
	int ret;
	ret=platform_driver_register(&myserial_driver);
	if(ret)
		printk("error in registering myserial driver\n");	
    printk("successfully registered dummy serial driver\n");
	return 0;

}
void __exit myserial_drv_exit(void)
{
	platform_driver_unregister(&myserial_driver);
	printk("dummy serial driver unregistered successfully\n");	
}

module_init(myserial_drv_init);
module_exit(myserial_drv_exit);

MODULE_LICENSE("GPL");

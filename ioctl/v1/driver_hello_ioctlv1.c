#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kfifo.h>
#include <linux/list.h> 
#include <sys/ioctl.h>

struct pseudo_stat
 {  
int len;  
int avail;
 };


#define IOC_MAGIC ‘p’ 
#define MY_IOCTL_LEN    _IO(IOC_MAGIC, 1) 
#define MY_IOCTL_AVAIL	_IO(IOC_MAGIC, 2) 
#define MY_IOCTL_RESET 	_IO(IOC_MAGIC, 3)
#define MY_IOCTL_PSTAT       _IOR(IOC_MAGIC, 4, struct pseudo_stat)


typedef struct priv_obj {
struct cdev cdev;
struct kfifo myfifo;
struct device* pdev;
struct list_head lentry;
}PRIV_OBJ;



LIST_HEAD(mydevlist);
int ndevices=1;
module_param(ndevices,int,S_IRUGO);
struct class *pcls;
unsigned char *pbuffer;
int rd_offset=0;
int wr_offset=0;
int buflen=0;
dev_t mydevid;

#define MAX_SIZE 1024
#define BASE_MINOR 0

int i;



int pseudo_open (struct inode * inode, struct file * file)
{	PRIV_OBJ *pobj = container_of(inode->i_cdev, PRIV_OBJ, cdev);
	file->private_data=pobj;
	printk("open sucessfully\n");
	return 0;
}

int pseudo_close (struct inode * inode, struct file * file)
{
	printk("released succcefully\n");
	return 0;
}

ssize_t pseudo_read (struct file * file, char __user * ubuf, size_t size, loff_t * off)
{ 
PRIV_OBJ *pobj = file->private_data;
int rcount,ret;
printk("read successfully\n");
if(kfifo_is_empty(&(pobj->myfifo))) //wr_offset-rd_offset==0
{
	printk("buffer is empty\n");
	return 0;
}
rcount = size;
if(rcount > kfifo_len(&(pobj->myfifo)))
	rcount = kfifo_len(&(pobj->myfifo));
	//min of buflen, size
char *tbuf;
tbuf = kmalloc(rcount, GFP_KERNEL);
kfifo_out(&(pobj->myfifo),tbuf,rcount);
ret=copy_to_user(ubuf,tbuf,rcount);
if(ret)
{
	printk("copy to user failed\n");
	return -EFAULT;
}
rd_offset+=rcount;
buflen -= rcount;
kfree(tbuf);
return rcount;	
}


ssize_t pseudo_write (struct file * file, const char __user * ubuf, size_t size, loff_t * off)
{ 
PRIV_OBJ *pobj = file->private_data;
int wcount,ret;
 printk("write successfully\n");
 if(kfifo_is_full(&(pobj->myfifo)))
 {
	printk("buffer is full\n");
	return -ENOSPC;
 }
 
 wcount = size;
 if(wcount > kfifo_avail(&(pobj->myfifo)))
	wcount = kfifo_avail(&(pobj->myfifo));
	//min
char *tbuf;
tbuf=kmalloc(wcount, GFP_KERNEL);
ret=copy_from_user(tbuf,ubuf,wcount);
if(ret)
{
	printk("copy from user failed\n");
	return -EFAULT;
}
kfifo_in(&(pobj->myfifo), tbuf, wcount);
kfree(tbuf);
return wcount; 	
}

static long pseudo_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
 { 
	int ret;  
	printk("Pseudo--ioctl method\n");  
	switch (cmd) {    
		case MY_IOCTL_LEN :             
			printk("ioctl--kfifo length is %d\n", kfifo_len(&myfifo));            
			break;    
		case MY_IOCTL_AVAIL:             
			printk("ioctl--kfifo avail is %d\n", kfifo_avail(&myfifo));            
			break;       
		case MY_IOCTL_RESET:             
			printk("ioctl--kfifo got reset\n");            
			kfifo_reset(&myfifo);            
			break;  
		case MY_IOCTL_PSTAT:            
			printk("ioctl--kfifo statistrics\n");           
 			stat.len=kfifo_len(&myfifo);            
			stat.avail=kfifo_avail(&myfifo);            
			ret=copy_to_user( (char __user*)arg, &stat, sizeof(pseudo_stat));            
			if(ret) 
			{                
				printk("error in copy_to_user\n"); 
				return -EFAULT;            
			} 
			break;

		}  
	return 0;
 }

struct file_operations fops = {
    .open = pseudo_open,
    .read = pseudo_read,
    .write = pseudo_write,
    .release = pseudo_close,
    .unlocked_ioctl = pseudo_ioctl
};

 



static int __init pseudo_init(void) {        //init_module
int ret,i=0;;
pcls = class_create(THIS_MODULE, "pseudo_class");
if(pcls==NULL)
{
    printk("class_create failed\n");
    return -EINVAL;
}
ret=alloc_chrdev_region(&mydevid, BASE_MINOR, ndevices, "pseudo_char");
if(ret) 
{
	printk("failed to register char driver");
  	return -EINVAL;
}
printk("successfully registered, major=%d\n",MAJOR(mydevid)); 
PRIV_OBJ* pobj;
for(i=0;i<ndevices;i++){
	pobj=kmalloc(sizeof(PRIV_OBJ), GFP_KERNEL);
	if(pobj==NULL)
		printk("pobj error");
	kfifo_alloc(&(pobj->myfifo), MAX_SIZE,  GFP_KERNEL);
	
	cdev_init(&(pobj->cdev), &fops);
	kobject_set_name(&(pobj->cdev.kobj),"pdevice%d",i);
	cdev_add(&(pobj->cdev), mydevid+i, 1); 


	pobj->pdev=device_create(pcls, NULL, mydevid+i, NULL, "psample%d",i);
	list_add_tail(&(pobj->lentry), &mydevlist);
}
  return 0;
}

static void __exit pseudo_exit(void) {       //cleanup_module
printk("bye bye");
	
	
//kfifo_free(&(pobj->myfifo));
	
struct list_head *pcur, *qcur;
int i=0;
PRIV_OBJ* pobj;
list_for_each_safe(pcur, qcur, &mydevlist)//per device cleanup
{
pobj = list_entry(pcur, PRIV_OBJ, lentry);
cdev_del(&pobj->cdev);
kfifo_free(&pobj->myfifo);
device_destroy(pcls, mydevid+i);
i++;
}
class_destroy(pcls);
unregister_chrdev_region(mydevid, ndevices);
kfree(pobj);
}


module_init(pseudo_init);
module_exit(pseudo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("rohith");
MODULE_DESCRIPTION("pseudo device Module");

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/usb.h>
 
#define MIN(a,b) (((a) <= (b)) ? (a) : (b))
#define BULK_EP_OUT 0x01
#define BULK_EP_IN 0x81
#define MAX_PKT_SIZE 64
#define VENDOR_ID	0x04D8
#define PRODUCT_ID	0x0053
 
static struct usb_device *device;
static struct usb_class_driver class;
//static unsigned char bulk_buf[MAX_PKT_SIZE];
 
static int pen_open(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Device sysfs was opened");
    return 0;
}
static int pen_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Device sysfs was closed");
    return 0;
}
static ssize_t pen_read(struct file *f, char __user *buf, size_t cnt, loff_t *off)
{
    int retval;
    int read_cnt;
 	unsigned char *bulk_buf;
 	bulk_buf = kmalloc(64, GFP_KERNEL);
    /* Read the data from the bulk endpoint */
    retval = usb_bulk_msg(device, usb_rcvbulkpipe(device, BULK_EP_IN),
            bulk_buf, MAX_PKT_SIZE, &read_cnt, 5000);
    if (retval)
    {
        printk(KERN_ERR "Read Bulk message returned %d\n", retval);
        kfree(bulk_buf);
        return retval;
    }
    if (copy_to_user(buf, bulk_buf, MIN(cnt, read_cnt)))
    {
    	kfree(bulk_buf);
        return -EFAULT;
    }
 	kfree(bulk_buf);
    return MIN(cnt, read_cnt);
}
static ssize_t pen_write(struct file *f, const char __user *buf, size_t cnt, loff_t *off)
{
    int retval;
    int wrote_cnt ;
    unsigned char *bulk_buf;
 	bulk_buf = kmalloc(64, GFP_KERNEL);
    if (copy_from_user(bulk_buf, buf,MAX_PKT_SIZE))
    {
    	printk(KERN_ERR "could not copy from user space");
    	kfree(bulk_buf);
        return -EFAULT;
    }
 
    /* Write the data into the bulk endpoint */
    retval = usb_bulk_msg(device, usb_sndbulkpipe(device, BULK_EP_OUT),
            bulk_buf, MAX_PKT_SIZE, &wrote_cnt, 5000);
    if (retval)
    {
        printk(KERN_ERR "write Bulk message returned %d\n", retval);
        kfree(bulk_buf);
        return retval;
    }
 	kfree(bulk_buf);
    return wrote_cnt;
}
 
static struct file_operations fops =
{
    .open = pen_open,
    .release = pen_close,
    .read = pen_read,
    .write = pen_write,
};
 
static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    int retval;
 
    device = interface_to_usbdev(interface);
 
    class.name = "Oman%d";
    class.fops = &fops;
    if ((retval = usb_register_dev(interface, &class)) < 0)
    {
        /* Something prevented us from registering this driver */
        printk(KERN_ERR "Not able to get a minor for this device.");
    }
    else
    {
        printk(KERN_INFO "Minor obtained: %d\n", interface->minor);
    }
 
    return retval;
}
 
static void pen_disconnect(struct usb_interface *interface)
{
    usb_deregister_dev(interface, &class);
}
 
/* Table of devices that work with this driver */
static struct usb_device_id pen_table[] =
{
    { USB_DEVICE(VENDOR_ID ,PRODUCT_ID) },
    {} /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, pen_table);
 
static struct usb_driver pen_driver =
{
    .name = "ECCE5215_Driver",
    .probe = pen_probe,
    .disconnect = pen_disconnect,
    .id_table = pen_table,
};
 
static int __init pen_init(void)
{
    int result;
 
    /* Register this driver with the USB subsystem */
    if ((result = usb_register(&pen_driver)))
    {
        printk(KERN_ERR "usb_register failed. Error number %d", result);
    }
    return result;
}
 
static void __exit pen_exit(void)
{
    /* Deregister this driver with the USB subsystem */
    usb_deregister(&pen_driver);
}
 
module_init(pen_init);
module_exit(pen_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sulaiman Salim Al Habsi");
MODULE_DESCRIPTION("Custom made USB Device Driver");
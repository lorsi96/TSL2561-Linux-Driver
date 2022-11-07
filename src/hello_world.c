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

/* ************************************************************************** */
/*                              Module Constants                              */
/* ************************************************************************** */
#define DEVICE_NAME "tsl2561"
#define CLASS_NAME "LightSensor_LinuxDriver"
#define DEV_MINOR_NUMBER 0
#define MINOR_BASE_NUMBER 0
#define STATIC_MAJOR_BASE_NUMBER  202  /*Only if static cdev region is used.*/
#define DEVICE_COUNT 1

/* ************************************************************************** */
/*                               Module Globals                               */
/* ************************************************************************** */
static struct cdev tls2561CDev;
static struct class *tls2561Class;
static dev_t tls2561Dev;

/* ************************************************************************** */
/*                              File IO Handlers                              */
/* ************************************************************************** */
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


/* ************************************************************************** */
/*                         Char Dev Region Strategies                         */
/* ************************************************************************** */
/**
 * @brief Static allocation of char device.
 * @param[out] dev_no obtained major version. Always the same for static alloc.
 * @return int error code.
 * @note this strategy of char dev region allocation is not ideal, since 
 *      it runs the risk of using the same major number as an already existing
 *      device. 
 */
int static_chrdev_region(dev_t* dev_no) {
	int ret = register_chrdev_region(STATIC_MAJOR_BASE_NUMBER, DEVICE_COUNT,
				  DEVICE_NAME);
	if(ret < 0) {
		return ret;
	}
	*dev_no = MKDEV(STATIC_MAJOR_BASE_NUMBER, DEV_MINOR_NUMBER);
	return ret;
}

/**
 * @brief Dynamic allocation of char device.
 * 
 * @param[out] dev_no obtained major version. Provided by the kernel. 
 * @return int error code.
 */
int dynamic_chrdev_region(dev_t* dev_no) {
	return alloc_chrdev_region(dev_no, MINOR_BASE_NUMBER, DEVICE_COUNT,
				  DEVICE_NAME);
}


/* ************************************************************************* */
/*                             Module Init & Exit                            */
/* ************************************************************************* */
static int __init tsl2561_init(void)
{
	int ret;
	struct device *tsl2561Device;
	pr_info("TSL2561 Light Sensor Init\n");

	ret = dynamic_chrdev_region(&tls2561Dev);
	if (ret < 0) {
		pr_info("Unable to allocate device's major base number\n");
		return ret;
	}
	pr_info("TSL2561 device allocated correctly with major number %d\n", MAJOR(tls2561Dev));


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
	device_destroy(tls2561Class, tls2561Dev);
	class_destroy(tls2561Class);
	cdev_del(&tls2561CDev);
	unregister_chrdev_region(tls2561Dev, DEVICE_COUNT);
}

module_init(tsl2561_init);
module_exit(tsl2561_exit);

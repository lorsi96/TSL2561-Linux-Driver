#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lucas Orsi");
MODULE_DESCRIPTION("TSL2561 Driver Example");
MODULE_VERSION("0.01");

/* ************************************************************************** */
/*                              Module Constants                              */
/* ************************************************************************** */
#define DEVICE_NAME "tsl2561"
#define DRIVER_NAME "tsl2561"
#define MINOR_BASE_NUMBER 0

static const struct of_device_id TSL2561_DT_IDS[] = {
	{ .compatible = "mse," DEVICE_NAME },
	{ /* sentinel*/ }
};

/* ************************************************************************* */
/*                                Module Types                               */
/* ************************************************************************* */
struct tsl2561_client {
	struct miscdevice *miscdev;
	struct file_operations *file_ops;
	struct i2c_driver *i2c_driver;
	struct i2c_client *i2c_client;
};

/* ************************************************************************* */
/*                             Private Functions                             */
/* ************************************************************************* */
static int tsl2561_dev_open(struct inode *inode, struct file *file);
static int tsl2561_dev_close(struct inode *inode, struct file *file);
static long tsl2561_dev_ioctl(struct file *file, unsigned int cmd,
			      unsigned long arg);

static int tsl2561_probe(struct i2c_client *client,
			 const struct i2c_device_id *id);
static int tsl2561_remove(struct i2c_client *client);

/* ************************************************************************* */
/*                             Static Definitions                            */
/* ************************************************************************* */
static struct file_operations __file_ops = {
	.owner = THIS_MODULE,
	.open = tsl2561_dev_open,
	.release = tsl2561_dev_close,
	.unlocked_ioctl = tsl2561_dev_ioctl,
};

static struct miscdevice __miscdev = {
	.minor = MINOR_BASE_NUMBER,
	.name = DEVICE_NAME,
	.fops = &__file_ops,
};

static struct i2c_driver __i2c_driver = { .probe = tsl2561_probe,
					  .remove = tsl2561_remove,
					  .driver = {
						  .name = DRIVER_NAME,
						  .owner = THIS_MODULE,
						  .of_match_table = of_match_ptr(
							  TSL2561_DT_IDS),
					  } };

static struct tsl2561_client __client = {
	.file_ops = &__file_ops,
	.i2c_driver = &__i2c_driver,
	.miscdev = &__miscdev,
	.i2c_client = NULL,
};

/* ************************************************************************** */
/*                            Function Definitions                            */
/* ************************************************************************** */

static int tsl2561_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	int err_code = 0;

	// Initialize misc framework device.
	err_code = misc_register(__client.miscdev);
	if (err_code != 0) {
		pr_err("Could not register misc device %s", DEVICE_NAME);
		return err_code;
	}
	pr_info("Successfully registered %s as misc device.", DEVICE_NAME);

	// Save I2C client reference.
	__client.i2c_client = client;

	return err_code;
}

static int tsl2561_remove(struct i2c_client *client)
{
	misc_deregister(__client.miscdev);
	pr_info("Successfully unregistered %s as misc device.", DEVICE_NAME);
	return 0;
}

/* ************************************************************************* */
/*                                IO Handlers                                */
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

/* ************************************************************************* */
/*                            Driver Registration                            */
/* ************************************************************************* */
MODULE_DEVICE_TABLE(of, TSL2561_DT_IDS);
module_i2c_driver(__i2c_driver);

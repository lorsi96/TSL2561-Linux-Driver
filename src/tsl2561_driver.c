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
#define DEVICE_ADDRESS 0x39
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

/* ************************************************************************** */
/*                                I2C Utilities                               */
/* ************************************************************************** */
static int tsl2561_read_reg(uint8_t reg, uint8_t nbytes)
{
	static uint8_t in; // Used as a buffer.;
	static int out;
	static struct i2c_msg msg[] = {
		{ .addr = DEVICE_ADDRESS, .flags = 0, .len = 1, .buf = &in },
		{ .addr = DEVICE_ADDRESS,
		  .flags = I2C_M_RD | I2C_M_RECV_LEN,
		  .len = 1,
		  .buf = (unsigned char *)&out }
	};
	in = reg | 0x80; // Mask for cmd.
	i2c_transfer(__client.i2c_client->adapter, msg, 2);
	pr_info("Read: in: %x, out: %x, len : %x", in, out, nbytes);
	return out;
}

static int tsl2561_write_reg(uint8_t reg, int value, uint8_t nbytes)
{
	static uint8_t in;
	static uint8_t regcpy;
	static struct i2c_msg msg[] = {
		{ .addr = DEVICE_ADDRESS, .flags = 0, .len = 1, .buf = &regcpy },
		{ .addr = DEVICE_ADDRESS, .flags = 0, .len = 1, .buf = &in }
	};
	msg[1].len = nbytes;
	regcpy = reg | 0x80; // Mask for cmd.
	in = value;
	i2c_transfer(__client.i2c_client->adapter, msg, 2);
	pr_info("Write: in: %x", in);
	return 0;
}

/* ************************************************************************* */
/*                                IO Handlers                                */
/* ************************************************************************* */
static struct {
	union {
		struct {
			enum { TSL2561_BYTE = 0, TSL2561_WORD = 1 } len : 8;
			enum { TSL2561_WR = 0, TSL2561_RD = 1 } op : 8;
			uint8_t reg;
			uint8_t val;
		};
		unsigned long raw;
	};
} __tsl2561_args;
static const long TSL2561_ERROR = 0x100000000;

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
	long ret;
	__tsl2561_args.raw = arg;
	ret = TSL2561_ERROR;
	if (cmd != 100) {
		return 0;
	}
	switch (__tsl2561_args.op) {
	case TSL2561_RD:
		ret = tsl2561_read_reg(__tsl2561_args.reg, __tsl2561_args.len);
		break;
	case TSL2561_WR:
		tsl2561_write_reg(__tsl2561_args.reg, __tsl2561_args.val,
				  __tsl2561_args.len);
		ret = 0;
		break;
	default:
		break;
	}
	pr_info("op %x; len %x; reg %x, var %x", __tsl2561_args.op,
		__tsl2561_args.len, __tsl2561_args.reg, __tsl2561_args.val);
	pr_info("[%s] is called. cmd = %d, arg = %lx\n", __func__, cmd, arg);
	return ret;
}

/* ************************************************************************* */
/*                            Driver Registration                            */
/* ************************************************************************* */
MODULE_DEVICE_TABLE(of, TSL2561_DT_IDS);
module_i2c_driver(__i2c_driver);

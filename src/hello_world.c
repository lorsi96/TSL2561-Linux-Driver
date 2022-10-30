#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lucas Orsi");
MODULE_DESCRIPTION("Simple Hello World Application");
MODULE_VERSION("0.01");


static int __time_var;
static int user_arg;

module_param(user_arg, int, 0);
MODULE_PARM_DESC(user_arg, "Some argument");

static int __init lkm_example_init(void) {
    printk(KERN_INFO "Hello, World! [%d] \n", user_arg);
    __time_var = ktime_get_seconds();
    pr_info("Time start");
    return 0;
}

static void __exit lkm_example_exit(void) {
    __time_var = ktime_get_seconds() - __time_var;
    pr_info("Time elapsed %d", __time_var);
    printk(KERN_INFO "Goodbye, World!\n");
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);

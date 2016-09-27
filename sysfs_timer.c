#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>

static int tv = 10;
static struct kobject *timer_object;
static struct timer_list c_timer;

static ssize_t tv_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t tv_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);

static struct kobj_attribute tv_attribute = __ATTR(tv, 0660, tv_show, tv_store);

static void timer_routine(unsigned long data)
{	
	printk(KERN_INFO "Timer called (%ld).\n", jiffies);
	mod_timer(&c_timer, jiffies + msecs_to_jiffies(tv * 1000));
	
	if (tv == 0) {
		del_timer(&c_timer);
		printk(KERN_INFO "Timer stopped\n");
	}
}

static ssize_t tv_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", tv);
}

static ssize_t tv_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	sscanf(buf, "%du", &tv);
	return count;
}

static int __init timer_init(void)
{
	int error = 0;
	printk(KERN_INFO "Module initialized successfully\n");
	
	timer_object = kobject_create_and_add("timer_object", kernel_kobj);
	if(!timer_object)
		return -ENOMEM;
	
	error = sysfs_create_file(timer_object, &tv_attribute.attr);
	if (error) {
		printk(KERN_INFO "failed to create the tv file in /sys/kernel/timer_object \n");
	}
	else {
		printk(KERN_INFO "Created tv file in /sys/kernel/timer_object \n");
	}
	
	setup_timer(&c_timer, timer_routine, 0);
	printk(KERN_INFO "Starting timer to fire in (%d) sec (%ld)\n", tv, jiffies);
	error = mod_timer(&c_timer, jiffies + msecs_to_jiffies(tv * 1000));
	if (error)
		printk("Timer error\n");
	

	return error;
}

static void __exit timer_exit(void)
{
	int error;
	error = del_timer(&c_timer);
	if (error)
		printk("Timer is in use\n");
	
	kobject_put(timer_object);
	printk(KERN_INFO "Module uninitialized successfully \n");
}

module_init(timer_init);
module_exit(timer_exit);
MODULE_LICENSE("GPL");

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/bug.h>

MODULE_AUTHOR("Mykhailo Bondarchuk");
MODULE_DESCRIPTION("Lab 6 - Kernel module debugging with BUG_ON");
MODULE_LICENSE("Dual BSD/GPL");

static uint print_count = 1;
module_param(print_count, uint, 0444);
MODULE_PARM_DESC(print_count, "Number of times to print");

struct hello_time_list {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(hello_time_head);

static int __init hello_init(void)
{
	uint i;

	/* Завдання 1.I: BUG_ON() замість повернення -EINVAL */
	BUG_ON(print_count > 10);

	if (print_count == 0 || (print_count >= 5 && print_count <= 10))
		pr_warn("Warning: count is 0 or between 5 and 10.\n");

	for (i = 0; i < print_count; i++) {
		struct hello_time_list *ptr;

		ptr = kmalloc(sizeof(*ptr), GFP_KERNEL);

		/* Завдання 1.II: Примусове внесення помилки kmalloc на останньому елементі */
		if (i == print_count - 1)
			ptr = NULL;

		BUG_ON(!ptr);

		ptr->time = ktime_get();
		list_add_tail(&ptr->list, &hello_time_head);

		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_time_list *md, *tmp;

	list_for_each_entry_safe(md, tmp, &hello_time_head, list) {
		pr_info("Event time (ns): %lld\n", ktime_to_ns(md->time));
		list_del(&md->list);
		kfree(md);
	}

	pr_info("Module unloaded.\n");
}

module_init(hello_init);
module_exit(hello_exit);

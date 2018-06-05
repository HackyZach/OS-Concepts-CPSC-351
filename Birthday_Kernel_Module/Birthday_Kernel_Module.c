/*
	Owen, Zachary
	zach.owen50@gmail.com
	CPSC 351 OS Concepts
	Professor Mccarthy
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

struct birthday
{
	int day;
	int month;
	int year;
	struct list_head list;
};

static LIST_HEAD(birthday_list);

void newBirthday(int month, int day, int year)
{
	struct birthday *bday;

	bday = kmalloc(sizeof(*bday), GFP_KERNEL);
	bday->day = day;
	bday->month = month;
	bday->year = year;
	INIT_LIST_HEAD(&bday->list);
	list_add_tail(&bday->list, &birthday_list);
}

int birthday_init(void)
{

	struct birthday *ptr;

	newBirthday(12, 18, 1988);
	newBirthday(12, 21, 1988);
	newBirthday(2, 1, 1991);
	newBirthday(4, 11, 1964);
	newBirthday(12, 24, 1961);

	list_for_each_entry(ptr, &birthday_list, list)
	{
		printk(KERN_INFO "Month %d, Day %d, Year %d\n", ptr->month, 
		ptr->day, ptr->year);
	}

       	return 0;
}


void birthday_exit(void) 
{
	struct birthday *ptr, *next;
	list_for_each_entry_safe(ptr, next, &birthday_list, list)
	{
		list_del(&ptr->list);
		kfree(ptr);
	}
}

module_init( birthday_init );
module_exit( birthday_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Birthday Module");
MODULE_AUTHOR("SGG");


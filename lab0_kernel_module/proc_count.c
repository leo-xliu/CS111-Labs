#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>

static int show(struct seq_file *m, void *v)
{
  int i = 0;
  struct task_struct *p;
  for_each_process(p){
    i++;
  }
  seq_printf(m, "%d\n", i);
  return 0;
}

static struct proc_dir_entry *count;
static int __init proc_count_init(void)
{
        count = proc_create_single("count", 0707, NULL, show);
	pr_info("proc_count: init\n");
	return 0;
}

static void __exit proc_count_exit(void)
{
        proc_remove(count);
        pr_info("proc_count: exit\n");
}

module_init(proc_count_init);
module_exit(proc_count_exit);

MODULE_AUTHOR("Leo Liu");
MODULE_DESCRIPTION("Module will print our total current running processes");
MODULE_LICENSE("GPL");

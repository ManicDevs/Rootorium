#include <linux/module.h>
#include <linux/kernel.h>

static int __init km_init(void)
{
    return 0;
}

static void __exit km_exit(void)
{

}

module_init( km_init );
module_exit( km_exit );

MODULE_LICENSE( "GPL" );

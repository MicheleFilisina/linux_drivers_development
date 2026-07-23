#include <linux/init.h>
#include <linux/module.h>
static char * whom = "world";
static int howmany = 1;
module_param(whom, charp, S_IRUGO);
module_param(howmany, int, S_IRUGO);


int pipooo = 676767;

EXPORT_SYMBOL(pipooo);

static int hello_init(void){
  for(; howmany>0; howmany--){
    printk(KERN_ALERT "Hello, %s\n", whom);
  }
  
  return 0;
}

static void hello_exit(void){
  printk(KERN_ALERT "Goodbye world\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("dummy module");
MODULE_AUTHOR("Michele Filisina");


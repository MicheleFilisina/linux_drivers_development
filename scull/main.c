#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /*kernel version of the standard library*/
#include <linux/fs.h> /*structures and function pointers to menage files*/

#Include <scull.h> // file with all definitions and macros

int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_nr_dev =
dev_t dev_number;

module_param(scull_major, int, S_IRUGO);



static char * whom = "world";
static int howmany = 1;
module_param(whom, charp, S_IRUGO);
module_param(howmany, int, S_IRUGO);



static int scull_init(void){
  dev_t dev_number;
  int result;
  
  if(scull_major){
    dev_number(scull_major, scull_minor);
    result = register_chrdev_region(dev_number, scull_nr_dev, "scull"); // statically register device number from scull_major
  }else{
    result = alloc_chrdev_region(&dev_number, scull_minor, scull_nr_dev, "scull"); // dinamically register device numbers, the major will be chosen randomically
    scull_major = MAJOR(dev);
  }
  
  return 0;
}

static void scull_cleanup(void){
  
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("symple scull driver");
MODULE_AUTHOR("Michele Filisina");


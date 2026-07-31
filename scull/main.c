#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /*kernel version of the standard library*/
#include <linux/fs.h> /*structures and function pointers to menage files*/
#include <linux/cdev.h> /*define a structure to rapresent a char device*/
#include <asm/uaccess.h> /*to deference a user-space buffer*/

#Include "scull.h" // file with all definitions and macros

int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_nr_dev = 1;
dev_t dev_number;

struct scull_dev * scull_devices; // scull_dev is a structure that rapresents the device and its internal data

struct cdev * scull;

module_param(scull_major, int, S_IRUGO);
module_param(scull_nr_dev, int, S_IRUGO);

struct file_operations scull_fops = {
  .owner = THIS_MODULE,
  .llseek = scull_llseek,
  .read = scull_read,
  .write = scull_write,
  .ioctl = scull_ioctl,
  .opem = scull_open,
  .release = scull_release,
};

// return the pointer to dev->data list item-th element
scull_qset * scull_follow(struct scull_dev * dev, int item){
  
  struct scull_qset *ptr = dev->data;

  //we initialize the first element of the list
  if(ptr==NULL){
    ptr = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
    if(ptr == NULL) return NULL; //allocation didn't end well
    memset(ptr, 0, sizeof(struct scull_qset));
  };
   
  while(item--){
    if (ptr->next == NULL) {
      ptr->next =  kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
      if(ptr==NULL) return NULL; //allocation didn't end well
      memset(ptr, 0, sizeof(struct scull_qset));
    }
    ptr = ptr->next;
  }

  return ptr;
  
}

static ssize_t scull_read(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos){
  struct scull_dev *dev = filp->private_data;
  struct scull_qset *ptr;
  int quantum = dev->quantum;
  int qset = dev->qset;
  int itemseize = quantum * qset; /* how many bytes in a qset*/
  int item, s_pos, q_pos, rest;
  ssize_t retval = 0;

  if (down_interruptible(&dev->sem))
       return -ERESTARTSYS;
   if (*f_pos >= dev->size)
       	goto out;
   if (*f_pos + count > dev->size)
       	count = dev->size - *f_pos;

   
   item = (long)*f_pos / itemsize; 
   rest = (long)*f_pos % itemsize; // position inside the quantum
   s_pos = rest / quantum; // the position of the quantum (the element )in the qset (the array)
   q_pos = rest % quantum; // the position in the quantum

   ptr = scull_follow(dev, item);

   if(ptr == NULL || ptr->data==NULL || ptr->data[s_pos]==NULL) goto out;

   if (count > quantum - q_pos) count = quantum - q_pos; // can only read one quantum at a time

   if(copy_to_user(buf, ptr->data[s_pos] + q_pos, count)){
     retval = -EFAULT;
     goto out;
   }

   *f_pos += count;
   retval = count;

 out:
   up(&dev->sem);
   return retval;
     
}

static ssize_t scull_qrite(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos){
  
}


static void setup_cdev(struct scull_dev *dev, int index){
  int err;
  int devno = MKDEV(scull_major, scull_minor + index);

  cdev_init(&dev->cdev, &scull_fops);
  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops = &scull_fops;
  err = cdev_add(&dev->cdev, devno,1);
  if (err) printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}

static void scull_open(struct inode *inode, struct file *filp){
  struct scull_dev *dev;
  dev = container_of(inode->i_cdev, struct scull_dev, cdev);
  filp->private_data = dev;

  // Reset the data of the device to 0 if open was write-only
  if((filp->f_flags & O_ACCMODE) == O_WRONLY){
    scull_trim(dev);
  }
  return 0;
}

void static scull_close(struct inode *inode, struct file *filp){
  return 0;
}

int static scull_trim(struct
		      scull_dev *dev){

  struct scull_qset * next, * ptr;
  int qset = dev->qset;

  for(ptr = dev->data; ptr!=NULL; ptr = next){
    if (ptr->data){
      for(int i = 0; i<qset; i++) kfree((ptr->data)[i]);
      kfree(ptr->data);
    }
    next = ptr->next;
    kfree(ptr);
  }

  dev->size = 0;
  dev->quantum = scull_quantum;
  dev->qset = scull_qset;
  dev->data = NULL;
  return 0;

  
}

static int scull_init(void){
  dev_t dev_number = 0;
  int result;
  
  if(scull_major){
    dev_number = MKDEV(scull_major, scull_minor);
    result = register_chrdev_region(dev_number, scull_nr_dev, "scull"); // statically register device number from scull_major
  }else{
    result = alloc_chrdev_region(&dev_number, scull_minor, scull_nr_dev, "scull"); // dinamically register device numbers, the major will be chosen randomically
    scull_major = MAJOR(dev_number);
  }
  if(result<0){
    printk(KERN_WARNING "scull: can't get major number %d \n", scull_major);
    return result;
  }

  // allocate the structures for deviced -- scull_nr_dev structures
  scull_devices = kmalloc(scull_nr_dev * sizeof(struct scull_dev), GFP_KERNEL);

  if(!*scull_devices){
    result = -ENOMEM;
    goto fail;
  }
  
  return 0;

 fail:
  scull_cleanup();
  return result;
}

static void scull_cleanup(void){
  
}

module_init(scull_init);
module_exit(scull_cleanup);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("symple scull driver");
MODULE_AUTHOR("Michele Filisina");


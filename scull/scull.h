#ifndef SCULL_MAJOR
#define SCULL_MAJOR 0
#endif

/*
the bere device is a variable-lenght region of memory.
it is besically a linked list of blocks called Scull_quset.
each block points to an array of pointer to a memory area of SCULL_QUANTUM bytes.

the array (aka quantum_set) is SCULL_QSET long.
 */

#ifndef SCULL_QUANTUM
#define SCULL_QUANTUM 4000
#endif

#ifndef SCULL_QSET
#define SCULL_QSET 1000
#endif


struct scull_qset {
  void ** data;
  struct scull_qset * next;
};

struct scull_dev {
  struct scull_qset *data;  /* Pointer to first quantum set */
  int quantum;              /* the current quantum size */
  int qset;                 /* the current array size */
  unsigned long size;       /* amount of data stored here */
  unsigned int access_key;  /* used by sculluid and scullpriv */
  struct semaphore sem;     /* mutual exclusion semaphore     */
  struct cdev cdev;	    /* Char device structure		*/
}

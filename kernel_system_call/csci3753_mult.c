#include <linux/kernel.h> 
#include <linux/linkage.h> 
#include <linux/uaccess.h>
 
asmlinkage long sys_csci3753_mult(int number1, int number2, long *result) 
{
  //initialize result variable and change input numbers to long
  long res, n1, n2;
  n1 = (long)number1;
  n2 = (long)number2;
  
  //first print statement of numbers being multiplied
  printk(KERN_ALERT "Numbers being multiplied:  %d and %d\n",  number1, number2);

  //store multiplied numbers as result
  res = n1 * n2; 

  //print the result  
  printk(KERN_ALERT "Result: %ld\n", res);

  //use copy_to_user to copy the address of the calculated result in the kernel to the address of the long result in the user space
  copy_to_user(result, &res, sizeof(long));

  return 0; 
}  

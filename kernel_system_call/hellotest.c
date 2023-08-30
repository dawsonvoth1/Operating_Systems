#include <unistd.h> 
#include <stdlib.h> 
 
int main() { 
  long int res = syscall(333); 
  exit(res); 
}

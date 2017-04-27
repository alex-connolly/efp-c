
#ifndef EFP_UTIL_H
#define EFP_UTIL_H


int is_power_of_two (unsigned int x){
  return ((x != 0) && ((x & (~x + 1)) == x));
}

#endif

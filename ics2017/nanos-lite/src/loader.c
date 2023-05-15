#include "common.h"
#include "fs.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
  /*
  extern size_t get_ramdisk_size();
  extern void ramdisk_read(void*,off_t,size_t);
  size_t len=get_ramdisk_size();
  ramdisk_read(DEFAULT_ENTRY,0,len);
  */

  int fd=fs_open(filename,0,0);
  Log("Filename=%s,fd=%d",filename,fd);
  fs_read(fd,DEFAULT_ENTRY,fs_filesz(fd));
  //Log("dsjk");
  fs_close(fd);

  return (uintptr_t)DEFAULT_ENTRY;
}




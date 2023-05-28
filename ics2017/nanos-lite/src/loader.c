#include "common.h"
#include "fs.h"
#include "memory.h"
#define DEFAULT_ENTRY ((void *)0x8048000)

uintptr_t loader(_Protect *as, const char *filename) {
  /*
  extern size_t get_ramdisk_size();
  extern void ramdisk_read(void*,off_t,size_t);
  size_t len=get_ramdisk_size();
  ramdisk_read(DEFAULT_ENTRY,0,len);
  */

  int fd=fs_open(filename,0,0);
  Log("Filename=%s,fd=%d",filename,fd);
  //fs_read(fd,DEFAULT_ENTRY,fs_filesz(fd));

  int size=fs_filesz(fd);
  int ppnum=size/PGSIZE;
  if(size%PGSIZE!=0) ppnum++;
  void *pa=NULL;
  void *va=DEFAULT_ENTRY;
  for(int i=0;i<ppnum;i++)
  {
      pa=new_page();
      _map(as,va,pa);
      fs_read(fd,pa,PGSIZE);
      va+=PGSIZE;
  }

  fs_close(fd);

  return (uintptr_t)DEFAULT_ENTRY;
}




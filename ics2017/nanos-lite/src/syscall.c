#include "common.h"
#include "syscall.h"
#include "fs.h"

int sys_none()
{
    return 1;
}

void sys_exit(int code)
{
    _halt(code);
}

uintptr_t sys_write(int fd,void *buf,size_t len)
{
    if(fd==1||fd==2)
    {
	char a;
        Log("output: %s",(char*)buf);
	for(int i=0;i<len;i++)
	{
	    memcpy(&a,(void*)buf+i,1);
	    _putc(a);
	}
	return len;
    }
    /*
    else // when fd=3?????
    {
	panic("Unhandled fd=%d in sys_write()",fd);
    }*/
    if(fd>=3)
    {
        return fs_write(fd,buf,len);
    }
    return -1;
}

int sys_brk(uintptr_t addr)
{
    return 0;
}

int sys_open(const char* filename)
{
    return fs_open(filename,0,0);
}

ssize_t sys_read(int fd,void* buf,size_t len)
{
    return fs_read(fd,buf,len);
}

int sys_lseek(int fd,off_t offset,int whence)
{
    return fs_lseek(fd,offset,whence);
}

int sys_close(int fd)
{
    return fs_close(fd);
}



_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);

  switch (a[0]) {
    case SYS_none:
        SYSCALL_ARG1(r)=sys_none();
	break;
    case SYS_exit:
        sys_exit(a[1]);
	break;
    case SYS_write:
        SYSCALL_ARG1(r)=sys_write(a[1],(void*)a[2],a[3]);
        break;
    case SYS_brk:
        SYSCALL_ARG1(r)=sys_brk(a[1]);
        break;
    case SYS_open:
        SYSCALL_ARG1(r)=sys_open((char*)a[1]);
        break;
    case SYS_read:
        SYSCALL_ARG1(r)=sys_read(a[1],(void*)a[2],a[3]);
	break;
    case SYS_lseek:
        SYSCALL_ARG1(r)=sys_lseek(a[1],a[2],a[3]);
	break;
    case SYS_close:
        SYSCALL_ARG1(r)=sys_close(a[1]);
	break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}

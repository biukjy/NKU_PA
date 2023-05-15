#include "fs.h"

extern void ramdisk_read(void *buf,off_t offset,size_t len);
extern void ramdisk_write(const void *buf,off_t offset,size_t len);
extern void dispinfo_read(void *buf,off_t offset,size_t len);
extern void fb_write(const void *buf,off_t offset,size_t len);
extern size_t events_read(void *buf,size_t len);


typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FD_FB].size=_screen.width*_screen.height*sizeof(uint32_t);


}

size_t fs_filesz(int fd)
{
    return file_table[fd].size;
}

off_t disk_offset(int fd)
{
    return file_table[fd].disk_offset;
}

off_t get_open_offset(int fd)
{
    return file_table[fd].open_offset;
}

void set_open_offset(int fd,off_t n)
{
    if(n>file_table[fd].size)
    {
        n=file_table[fd].size;
    }
    file_table[fd].open_offset=n;
}

int fs_open(const char *pathname,int flags,int mode)
{
    for(int i=0;i<NR_FILES;i++)
    {
	if(strcmp(file_table[i].name,pathname)==0)
	{
	    file_table[i].open_offset=0;
	    return i;
	}
    }
    panic("this filename not exists!");
    return -1;
}

ssize_t fs_read(int fd,void *buf,size_t len)
{
    assert(fd>=0&&fd<NR_FILES);
    if(fd<3||fd==FD_FB)
    {
	return 0;
    }
    if(fd==FD_EVENTS)
    {
	return events_read(buf,len);
    }

    ssize_t n=fs_filesz(fd)-get_open_offset(fd);
    if(n>len){ n=len; }

    if(fd==FD_DISPINFO)
    {
	dispinfo_read(buf,get_open_offset(fd),n);
    }
    else
    {
	ramdisk_read(buf,disk_offset(fd)+get_open_offset(fd),n);
    }

    set_open_offset(fd,get_open_offset(fd)+n);
    return n;


}

int fs_close(int fd)
{
    return 0;
}

ssize_t fs_write(int fd,const void *buf,size_t len)
{
    assert(fd>=0&&fd<NR_FILES);
    if(fd<3||fd==FD_DISPINFO)
    {
	return 0;
    }

    ssize_t n=fs_filesz(fd)-get_open_offset(fd);
    if(n>len){ n=len; }

    if(fd==FD_FB)
    {
	fb_write(buf,get_open_offset(fd),n);
    }
    else
    {
	ramdisk_write(buf,disk_offset(fd)+get_open_offset(fd),n);
    }

    set_open_offset(fd,get_open_offset(fd)+n);
    return n;
}

off_t fs_lseek(int fd,off_t offset,int whence)
{
    switch (whence)
    {
	case SEEK_SET:
	    set_open_offset(fd,offset);
	    return get_open_offset(fd);
	case SEEK_CUR:
	    set_open_offset(fd,get_open_offset(fd)+offset);
	    return get_open_offset(fd);
	case SEEK_END:
	    set_open_offset(fd,fs_filesz(fd)+offset);
	    return get_open_offset(fd);
	default:
	    panic("whence error.\n");
	    return -1;
    }
}





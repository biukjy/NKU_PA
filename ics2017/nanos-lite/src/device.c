#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
    Log("2222\n");
    int key=_read_key();
    Log("8888\n");
    char buffer[40];
    int down=0;
    if(key&0x8000)
    {
	key^=0x8000;
	down=1;
    }
    Log("9888\n");
    if(key!=_KEY_NONE)
    {
	sprintf(buffer,"%s %s\n",down?"kd":"ku",keyname[key]);
    }
    else
    {
	sprintf(buffer,"t %d\n",_uptime());
    }
    Log("1888\n");
    if(strlen(buffer)<=len)
    {
	strncpy((char*)buf,buffer,strlen(buffer));
	return strlen(buffer);
    }
    return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
    strncpy(buf,dispinfo+offset,len);
}

extern void getScreen(int *p_wigth,int *p_height);
void fb_write(const void *buf, off_t offset, size_t len) {
    int index,x1,y1,y2;
    int width=0,height=0;

    getScreen(&width,&height);
    index=offset/4;
    y1=index/width;
    x1=index%width;
    index=(offset+len)/4;
    y2=index/width;

    if(y2==y1)
    {
	_draw_rect(buf,x1,y1,len/4,1);
	return;
    }
    
    int tempw=width-x1;
    if(y2-y1==1)
    {
	_draw_rect(buf,x1,y1,tempw,1);
	_draw_rect(buf+tempw*4,0,y2,len/4-tempw,1);
	return;
    }

    _draw_rect(buf,x1,y1,tempw,1);
    int tempy=y2-y1-1;
    _draw_rect(buf+tempw*4,0,y1+1,width,tempy);
    _draw_rect(buf+tempw*4+tempy*width*4,0,y2,len/4-tempw-tempy*width,1);

}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  int width=0,height=0;
  getScreen(&width,&height);
  sprintf(dispinfo,"WIDTH:%d\nHEIGHT:%d\n",width,height);
}

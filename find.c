/*
*文件功能：文件查找
*历史来源：四字班方案一代码
*整合作者：赵哲晖
*整合时间：2018/05/09
*/

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "date.h"

char* getFnameFromPath(char* path,char* fname)
{
	int i,j;
	i = 0;
	int len = strlen(path);
	for(i = 0; i < len; i++)
	{
		for(j = i; j  < len; j++)
		{
			if((j == 0 && path[j] == '.')  ||  (path[j] == '/'))
			{
				break;
			}
		}
		if(j == len)
		{
			break;
		}
	}
	if(i == len)
		return ".";

	int Len = (len - i);
	//char fname[100];
	for(j = 0;j < Len; j++)
	{
		fname[j] = path[i+j];
	}
	fname[Len] = 0;
	return fname;
}
	


int compare(char *c1,char* c2)
{
	int len1, len2;
	len1 = strlen(c1);
	len2 = strlen(c2);
	if(len1 != len2)
	{
		return -1;
	}
	int i;
	for(i = 0; i < len1; i++)
	{
		if(c1[i] != c2[i])
		{
			return -1;
		}
	}
	return 0;
}

char*
	fmtname(char *path)
{
	static char buf[DIRSIZ+1];
	char *p;

	// Find first character after last slash.
	for(p=path+strlen(path); p >= path && *p != '/'; p--)
		;
	p++;

	// Return blank-padded name.
	if(strlen(p) >= DIRSIZ)
		return p;
	memmove(buf, p, strlen(p));
	memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
	return buf;
}

void
ls(char *path)
{
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if((fd = open(path, 0)) < 0){
		printf(2, "ls: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		printf(2, "ls: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type)
	{
	case T_FILE:
		printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
		break;

	case T_DIR:
		if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
		{
			printf(1, "ls: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf+strlen(buf);
		*p++ = '/';
		while(read(fd, &de, sizeof(de)) == sizeof(de))
		{
			if(de.inum == 0)
				continue;
			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = 0;
			if(stat(buf, &st) < 0)
			{
				printf(1, "ls: cannot stat %s\n", buf);
				continue;
			}
			printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
		}
		break;
	}
	close(fd);
}

/*
*函数功能：uint转化为实时时钟
*作者：赵哲晖
*时间：2018/05/15
*/
rtcdate* timestampToDate(uint stamp)
{
  uint ret = 946684800; //utc+0 2000/1/1 0:0:0 946684800
  uint YEAR_SEC = 31536000;
  uint DAY_SEC = 86400;
  stamp -= ret;
  int years = 0;
  //get years
  while(1)
  {
    if(years != 0 && years % 4 == 0)
    {
      if(stamp >= YEAR_SEC + DAY_SEC)
      {
        stamp = stamp - (YEAR_SEC + DAY_SEC);
        years++;
      }
      else
        break;
    }
    else
    {
      if(stamp >= YEAR_SEC)
      {
        stamp -= YEAR_SEC;
        years++;
      }
      else
        break;
    }
  }
  //get month
  int days[]={31,28,31,30,31,30,31,31,30,31,30,31};
  if(years != 0 && years % 4 == 0)
    days[1] = 29;
  int month = 1;
  while(1)
  {
    int temp = stamp - days[month-1] * DAY_SEC;
    if(temp >= 0)
    {
      stamp = temp;
      month++;
    }  
    else
      break;
  }
  //get days
  int day = 1;
  while(stamp >= DAY_SEC)
  {
    stamp -= DAY_SEC;
    day++;
  }
  //get hours
  int hour = 0;
  while(stamp >= 3600)
  {
    stamp -= 3600;
    hour++;
  }
  //get minutes
  int minute = 0;
  while(stamp >= 60)
  {
    stamp -= 60;
    minute++;
  }
  int second = stamp;
  rtcdate * date = malloc(sizeof(rtcdate));
  date->year = years + 2000;
  date->month = month;
  date->day = day;
  date->hour = hour;
  date->minute = minute;
  date->second = second;
  return date;
}

void sf(char *path, char *fname)
{
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if((fd = open(path, 0)) < 0)
	{
		printf(2, "ls: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		printf(2, "ls: cannot stat %s\n", path);
		close(fd);
		return;
	}
	switch(st.type)
	{
	case T_FILE:
		{
			char name[100];
			if(compare(getFnameFromPath(path,name),fname) == 0)
			{
				rtcdate * date = timestampToDate(st.ctime);
				printf(1, "%s path:%s  size:%d  time: %d/%d/%d %d:%d:%d  %d\n", fname, path,st.size,date->year,date->month,date->day,date->hour,date->minute,date->second,st.ctime);
				free(date);
			}
			//printf(1,"%s %d %d", getFnameFromPath(path,name), compare(getFnameFromPath(path,name),"."), compare(getFnameFromPath(path,name),".."));
			//printf(0,"1/n");
			if((st.type == 1) && (compare(getFnameFromPath(buf,name),".") != 0) && (compare(getFnameFromPath(buf,name),"..") != 0))
			{
				sf(getFnameFromPath(path,name), fname);

			}
			break;
		}
	

	case T_DIR:
		if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
		{
			printf(1, "ls: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf+strlen(buf);
		*p++ = '/';
		while(read(fd, &de, sizeof(de)) == sizeof(de)){
			if(de.inum == 0)
				continue;
			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = 0;
			if(stat(buf, &st) < 0){
				printf(1, "ls: cannot stat %s\n", buf);
				continue;
			}
			char name[100];
			if(compare(getFnameFromPath(buf,name),fname) == 0)
			{
				rtcdate * date = timestampToDate(st.ctime);
				printf(1, "%s path:%s  size:%d  time: %d/%d/%d %d:%d:%d  %d\n", fname, path,st.size,date->year,date->month,date->day,date->hour,date->minute,date->second,st.ctime);
				free(date);
			}
			if((st.type == 1) && (compare(getFnameFromPath(buf,name),".") != 0) && (compare(getFnameFromPath(buf,name),"..") != 0))
			{
				//printf(1,"%s %d %d", getFnameFromPath(buf,name), compare(getFnameFromPath(buf,name),"."), compare(getFnameFromPath(buf,name),".."));
				//printf(0,"2");
				sf(getFnameFromPath(buf,name), fname);

			}
		}
		break;
	}
	close(fd);
}


int main(int argc, char *argv[])


{
	if(argc < 2)
	{
		printf(1, "please input the filename you want to find...\n");
		exit();
	}
	/*int i;
	for(i=1; i<argc; i++)
	ls(argv[i]);*/
	sf(".",argv[1]);
	exit();
}



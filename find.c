#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char* getFnameFromPath(char* path,char* fname)
{
	int i,j;
	i = 0;
	int len = strlen(path);
	for(i = 0; i < len; i++)
	{
		for(j = i; j  < len; j++)
		{
			if((path[j] == '.')  ||  (path[j] == '/'))
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
				printf(1, "%s path:%s  size:%d  time:%d\n", fname, path,st.size,st.ctime);
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
				printf(1,"%s path:%s  size:%d  time:%d\n", fname, buf, st.size,st.ctime);
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



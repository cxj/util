/* Put in the public domain by Kevet Duncombe, 1988 */
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include <string.h>

#ifdef NeXT
#include <sys/dir.h>
typedef struct direct Entry;
#else
#include <dirent.h>
typedef struct dirent Entry;
#endif

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#ifndef GREP
/* /usr/bin/grep for Darwin */
#define GREP "/usr/bin/grep"
#endif
#ifndef GREP
/* /bin/grep for Linux */
#define GREP "/bin/grep"
#endif

#ifndef STDDIR
#define STDDIR "/usr/include"
#endif

char **Argv;
char path[PATH_MAX+PATH_MAX+1] = ".";

int grepmask;		/* bitmask of file type to scan */
int recurse = 1;	/* recurse into subdirectories */

#define OTHER		1
#define SOURCE		2
#define HEADER		4
#define MAKEFILE	8

struct FileType {
	char *str;
	int type;
} filetype[] = {
	{".c",SOURCE},
	{".h",HEADER},
	{".cpp",SOURCE},
	{".CPP",SOURCE},
	{"makefile",MAKEFILE},
	{"Makefile",MAKEFILE},
	{"build.xml",MAKEFILE},
	{"pom.xml",MAKEFILE},
	{".C",SOURCE},
	{".pl",SOURCE},
	{".pm",SOURCE},
	{".php",SOURCE},
	{".inc",HEADER},
	{".php3",SOURCE},
	{".php4",SOURCE},
    {".module",SOURCE},
	{".java",SOURCE},
	{".tcl",SOURCE},
	{".mk",MAKEFILE},
	{".mak",MAKEFILE},
	{".MAK",MAKEFILE},
	{".hh",HEADER},
	{".i",HEADER},
	{".imp",HEADER},
	{".inc",HEADER},
	{".hpp",HEADER},
	{".hxx",HEADER},
	{".cxx",SOURCE},
	{".c++",SOURCE},
	{".h++",HEADER},
	{".l",SOURCE},
	{".y",SOURCE},
	{".cc",SOURCE},
	{".CC",SOURCE},
	{".HH",HEADER},
	{".s",SOURCE},
	{".asm",SOURCE},
	{".cs",SOURCE},
    {".js",SOURCE},
};
#define FILECLASSES	(sizeof(filetype)/sizeof(filetype[0]))

int
Typify(const char *file)
{
	int i;
	char *ext = strrchr(file,'.');
	/*  NOTE: we've already filtered out files that start with
	 *  dot so there's no conflict between filename matches and
	 *  extension matches (not that it was very likely anyway).
	 */
	for(i = 0; i < FILECLASSES && filetype[i].str; ++i)
	{
		if (strcmp(file,filetype[i].str) == 0)
			return filetype[i].type;
		if (ext && strcmp(ext,filetype[i].str) == 0)
			return filetype[i].type;
	}
	return OTHER;
}

void
GrepFile()
{
	int n;
	int pid = vfork();
	if (pid < 0) {          /* fork failed: still gr */
		fprintf(stderr,"fork failed, errno=%d\n",errno);
		exit(0);
	}
	else if (pid == 0)      /* fork worked: now in grep */
	{
		n = execv(GREP,Argv);
		/* we only get here if exec failed */
		write(2,"exec failed!\n",13);
		exit(errno);
	}
	else {			/* fork worked: still in gr */
		wait(0);
	}
}

void
ScanDir(char *tail)
{
	DIR *dirp;
	Entry *entry;

    if ((tail - path) > PATH_MAX)
    {
        fprintf(stderr, "path too long!");
        exit(1);
    }

	dirp = opendir(path);
	if (dirp == 0)
		return;
	*tail = '/';
	while((entry = readdir(dirp)) != NULL)
	{
		struct stat statbuf;
		if (entry->d_name[0] == '.')
			continue;
		strcpy(tail+1,entry->d_name);
		if (stat(path, &statbuf))
			continue;
		if (statbuf.st_mode & S_IFREG)
		{
			if (Typify(tail+1) & grepmask)
				GrepFile();
		}
	}
	rewinddir(dirp);
	if (recurse)
	{
		while((entry = readdir(dirp)) != NULL)
		{
			struct stat statbuf;
			if (entry->d_name[0] == '.')
				continue;
			if (strcmp(entry->d_name,".svn") == 0)
				continue;
			if (strcmp(entry->d_name,"CVS") == 0)
				continue;
			if (strcmp(entry->d_name,"RCS") == 0)
				continue;
			if (strcmp(entry->d_name,"SCCS") == 0)
				continue;
            if (strcmp(entry->d_name, ".git") == 0)
                continue;
			strcpy(tail+1,entry->d_name);
            /* use lstat so we can't follow symlinks and get loops */
			if (lstat(path, &statbuf))
				continue;
			if (statbuf.st_mode & S_IFDIR)
				ScanDir(tail+strlen(tail));
		}
	}
	closedir(dirp);
	*tail = '\0';
}

main(int argc, char *argv[])
{
	int i,len;
	char *str;
	str = strrchr(argv[0],'/');
	str = (str == 0) ? argv[0] : str+1;
	switch(str[2])
	{
	case 's': grepmask = SOURCE; break;
	case 'h': grepmask = HEADER; break;
	case 'i': grepmask = HEADER; strcpy(path,STDDIR); break;
	case 'm': grepmask = MAKEFILE; break;
	case '.': grepmask = MAKEFILE | SOURCE | HEADER; recurse = 0; break;
	case 'o': grepmask = OTHER; break;
	case 'a': grepmask = OTHER | MAKEFILE | SOURCE | HEADER; break;
	default:  grepmask = MAKEFILE | SOURCE | HEADER; break;
	}
	if (argc < 2 || strcmp(argv[1],"-?") == 0)
	{
		fprintf(stderr,
			"gr: recursively grep sourcecode files using %s\n",
			GREP);
		fprintf(stderr,"$Revision: 6557 $ of $Date: 2007-09-25 03:39:59 +0200 (Tue, 25 Sep 2007) $\n");
		fprintf(stderr,"These file types will be grep'ed:\n  ");
		for(i = len = 0; i < FILECLASSES; ++i)
		{
			if ((filetype[i].type & grepmask) == 0)
				continue;
			fprintf(stderr,"%s ",filetype[i].str);
			len += strlen(filetype[i].str)+1;
			if (len < 32)
				continue;
			len = 0;
			fprintf(stderr,"\n  ");
		}
		fprintf(stderr,"\n");
		fprintf(stderr,"Invoke as grs to limit to sources\n");
		fprintf(stderr,"Invoke as grh to limit to headers\n");
		fprintf(stderr,"Invoke as grm to limit to makefiles\n");
		fprintf(stderr,"Invoke as gr. to limit to current directory\n");
		fprintf(stderr,"Invoke as gri to limit to headers in %s\n",STDDIR);
		fprintf(stderr,"Invoke as gro to limit to other file types\n");
		fprintf(stderr,"Invoke as gra to grep all file types\n");
		exit(1);
	}
	Argv = (char**)calloc(argc+3,sizeof(char*));
	Argv[0] = GREP;
	for(i = 1; i < argc; ++i)
		Argv[i] = argv[i];
	Argv[i++] = path;
	Argv[i++] = "/dev/null";
	Argv[i] = 0;
	ScanDir(path+strlen(path));
	exit(0);
}

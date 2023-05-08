#define BUFSIZE 32768

#include <stdio.h>
#include <string.h>
#define UNLIKELY(check) __builtin_expect(check,0)
#define LINE(varname,string_literal) static const char varname[sizeof string_literal]=string_literal"\n"
#define DIEIF(check,string_literal) if(UNLIKELY(check)){LINE(e,string_literal);fwrite(e,1,sizeof e,stderr);return 1;}
#define READ(size) if(UNLIKELY(fread(buf,1,size,stdin)!=size))goto readfail
#define WRITE(buf,size) if(UNLIKELY(fwrite(buf,1,size,stdout)!=size))goto writefail

int main(const int argc,const char**const argv){
	if(UNLIKELY(argc>1)){
		DIEIF(argc!=2||strcmp("--help",argv[1]),"zip2gzip only understands --help or no arguments");
		LINE(help,"usage: zip2gzip < file.zip > file.gz\n\
note: `< file.zip > file.gz` are shell redirections, not arguments\n\
convert first file in zip archive to gzip without recompressing\n\
doesn't work with all zip archives\n\
github: https://github.com/xBZZZZ/zip2gzip");
		WRITE(help,sizeof help);
	}else{
		unsigned char buf[BUFSIZE],gziptrailer[8];
		READ(30);
		DIEIF(memcmp("PK\3\4",buf,4),"bad zip local file header signature");
		DIEIF(buf[6]&8,"zip data descriptor not supported");
		DIEIF(memcmp("\b",buf+8,2),"zip compression method is not deflate");
		memcpy(gziptrailer,buf+14,4);
		memcpy(gziptrailer+4,buf+22,4);
		unsigned long towrite=(unsigned long)buf[18]|(unsigned long)buf[19]<<8|(unsigned long)buf[20]<<16|(unsigned long)buf[21]<<24,
		totrash=((unsigned long)buf[26]|(unsigned long)buf[27]<<8)+((unsigned long)buf[28]|(unsigned long)buf[29]<<8);
		while(totrash>BUFSIZE){
			totrash-=BUFSIZE;
			READ(BUFSIZE);
		}
		if(totrash)READ(totrash);
		static const char gzipheader[10]="\37\213\b\0\0\0\0\0\0\377";
		WRITE(gzipheader,10);
		while(towrite>BUFSIZE){
			towrite-=BUFSIZE;
			READ(BUFSIZE);
			WRITE(buf,BUFSIZE);
		}
		if(towrite){
			READ(towrite);
			WRITE(buf,towrite);
		}
		WRITE(gziptrailer,8);
	}
	if(!UNLIKELY(fflush(stdout)))return 0;
writefail:;
	LINE(writefailmsg,"failed to write to stdout");
	fwrite(writefailmsg,1,sizeof writefailmsg,stderr);
	return 1;
readfail:;
	LINE(readfailmsg,"failed to read from stdin");
	fwrite(readfailmsg,1,sizeof readfailmsg,stderr);
	return 1;
}
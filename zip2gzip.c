#define BUFSIZE 32768

#include <stdio.h>
#include <string.h>
#include <assert.h>

static_assert(sizeof(char)==1,"8bit char");
static_assert(sizeof(unsigned char)==1,"8bit unsigned char");
static_assert(sizeof(unsigned)>=4,"32bit or larger unsigned int");

int main(const int argc,const char**argv){
	unsigned char buf[BUFSIZE],gziptrailer[8];
	char gzipheader[10]="\37\213\b\0\0\0\0\0\0\377";
#define UNLIKELY(check) __builtin_expect(check,0)
#define LINE(varname,string_literal) static const char varname[sizeof string_literal]=string_literal"\n"
#define DIEIF(check,string_literal) if(UNLIKELY(check)){LINE(e,string_literal);fwrite(e,1,sizeof e,stderr);return 1;}
#define READ(size) if(UNLIKELY(fread(buf,1,size,stdin)!=size))goto readfail
#define WRITE(buf,size) if(UNLIKELY(fwrite(buf,1,size,stdout)!=size))goto writefail
	if(argc>1){
		const char*infilepath=NULL,*outfilepath=NULL,*osstr=NULL,*arg;
		while(arg=*++argv){
			if(UNLIKELY(*arg!='-')){
				setvbuf(stderr,buf,_IOFBF,BUFSIZE);
				static const char e[33]="argument doesn't start with `-`: ";
				fwrite(e,1,33,stderr);
				fputs(arg,stderr);
				fputc('\n',stderr);
				fclose(stderr);
				return 1;
			}
			DIEIF(!*++arg,"bad argument: -");
another_letter:
			switch(*arg){
#define VAL_ARG(letter,varname) case letter:if(*++arg)varname=arg;else if(UNLIKELY(!(varname=*++argv)))break;continue
			VAL_ARG('i',infilepath);
			VAL_ARG('o',outfilepath);
			VAL_ARG('s',osstr);
#undef VAL_ARG
			case 't':
				gzipheader[3]=1;
				if(*++arg)goto another_letter;
				continue;
			case 'h':
			case '-':
				setvbuf(stdout,NULL,_IONBF,0);
				LINE(help,"convert first file in zip archive to gzip without recompressing\n\
doesn't work with all zip archives\n\
github: https://github.com/xBZZZZ/zip2gzip\n\
gzip spec: http://zlib.org/rfc-gzip.html\n\
parameters:\n\
  -i{input file path}   set input (zip) file (default stdin)\n\
  -o{output file path}  set output (gzip) file (default stdout)\n\
  -s{OS byte}           set OS byte (default -s255)\n\
  -t                    set FTEXT bit to 1\n\
  -h or --              write this help to stdout");
				WRITE(help,sizeof help);
				return 0;
			default:;
				char arg_err[24]="bad parameter letter: ?\n";
				arg_err[22]=*arg;
				fwrite(arg_err,1,24,stderr);
				return 1;
			}
			char missing_err[37]="missing value for -? (last argument)\n";
			missing_err[19]=*--arg;
			fwrite(missing_err,1,37,stderr);
			return 1;
		}
		if(osstr&&UNLIKELY(sscanf(osstr,"%hhi",gzipheader+9)!=1)){
			setvbuf(stderr,buf,_IOFBF,BUFSIZE);
			static const char e[25]="failed to parse os byte: ";
			fwrite(e,1,25,stderr);
			fputs(osstr,stderr);
			goto newline_and_die;
		}
		if(infilepath&&UNLIKELY(!freopen(infilepath,"r",stdin))){
			setvbuf(stderr,buf,_IOFBF,BUFSIZE);
			static const char e[27]="failed to open input file: ";
			fwrite(e,1,27,stderr);
			fputs(infilepath,stderr);
			goto newline_and_die;
		}
		if(outfilepath&&UNLIKELY(!freopen(outfilepath,"w",stdout))){
			setvbuf(stderr,buf,_IOFBF,BUFSIZE);
			static const char e[28]="failed to open output file: ";
			fwrite(e,1,28,stderr);
			fputs(outfilepath,stderr);
			goto newline_and_die;
		}
	}
	setvbuf(stdout,NULL,_IONBF,0);
	setvbuf(stdin,NULL,_IONBF,0);
	READ(30);
	DIEIF(memcmp("PK\3\4",buf,4),"bad zip local file header signature");
	DIEIF(buf[6]&8,"zip data descriptor not supported");
	DIEIF(memcmp("\b",buf+8,2),"zip compression method is not deflate");
	memcpy(gziptrailer,buf+14,4);
	memcpy(gziptrailer+4,buf+22,4);
	unsigned towrite=(unsigned)buf[18]|(unsigned)buf[19]<<8|(unsigned)buf[20]<<16|(unsigned)buf[21]<<24,
	totrash=((unsigned)buf[26]|(unsigned)buf[27]<<8)+((unsigned)buf[28]|(unsigned)buf[29]<<8);
	while(UNLIKELY(totrash>BUFSIZE)){
		totrash-=BUFSIZE;
		READ(BUFSIZE);
	}
	if(totrash)READ(totrash);
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
	return 0;
newline_and_die:
	fputc('\n',stderr);
	fclose(stderr);
	return 1;
writefail:;
	LINE(writefailmsg,"failed to write to output file");
	fwrite(writefailmsg,1,sizeof writefailmsg,stderr);
	return 1;
readfail:;
	LINE(readfailmsg,"failed to read from input file");
	fwrite(readfailmsg,1,sizeof readfailmsg,stderr);
	return 1;
}
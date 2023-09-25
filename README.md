I made this because I wanted to use [`kzip`](https://www.jonof.id.au/kenutils.html)'s deflate engine but `kzip` only creates `zip` file.
## `zip2gzip -h`
```
convert first file in zip archive to gzip without recompressing
doesn't work with all zip archives
github: https://github.com/xBZZZZ/zip2gzip
gzip spec: http://zlib.org/rfc-gzip.html
parameters:
  -i{input file path}   set input (zip) file (default stdin)
  -o{output file path}  set output (gzip) file (default stdout)
  -s{OS byte}           set OS byte (default -s255)
  -t                    set FTEXT bit to 1
  -h or --              write this help to stdout
```
## how to compile
```bash
export 'CC=zig cc -target x86_64-linux-gnu' #or whatever c compiler you want to use
./build.sh
```
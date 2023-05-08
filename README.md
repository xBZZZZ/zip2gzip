I made this because I wanted to use [`kzip`](https://www.jonof.id.au/kenutils.html)'s deflate engine but `kzip` only creates `zip` file.
## `zip2gzip --help`
```
usage: zip2gzip < file.zip > file.gz
note: `< file.zip > file.gz` are shell redirections, not arguments
convert first file in zip archive to gzip without recompressing
doesn't work with all zip archives
github: https://github.com/xBZZZZ/zip2gzip
```
## how to compile
```bash
CC='zig cc' #or whatever c compiler you want to usage
./build.sh
```
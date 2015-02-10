# ieatgor
program to parse gor files

A program i wrote at decode where they stored lots of data in format they called gor. It is a flat file with information belong to a single site on the genome. The first two columns are chromsome and position. The rest of the information in the file can be anything

## To compile
g++ -O3 -o ieatgor ieatgor.cpp -lz

## Run
./ieatgor

RUN:	ieatgor <targetFile> <file> OPTIONS

target file format:	 chrName:start-end

OPTIONS:
	-offset INT	 offset the target positions by INT
	-skip INT	 number of lines to skip (will be printet)
	-num INT	 are the chromosome in lexical (default 0) or numeric (1) order 

## Target file
The target file must be sorted in the same order as the file you want to extract from (gor file). example of target file

chr1:22323:232322
chr10:10000:10000000
chr3:10000:10000000
 
## Gor file

A "gor" file is a file with the first column being the chromosome and the second column being the positions. The rest of the columns are not important. The gor file must be sorted based on the positions and chr. gz gor files are also accepted. The sorting must be lexical or numeric example of a gor file

chr1    1033267 2       3       know 0.000000 EM 0.000000 unk 0.000000 EMunk 0.000000 Nind 9
chr1    1033268 0       2       know 0.000000 EM 0.000000 unk 0.000000 EMunk 0.000000 Nind 9
chr1    1033279 3       1       know 0.000000 EM 0.000000 unk 0.000000 EMunk 0.000000 Nind 9
chr1    1033286 0       2       know 0.000000 EM 0.000000 unk 0.000000 EMunk 0.000000 Nind 9
chr1    1033297 0       2       know 0.000000 EM 0.000000 unk 0.000000 EMunk 0.000000 Nind 9
chr1    1033304 0       1       know 0.000000 EM 0.000000 unk 0.000000 EMunk 0.000000 Nind 9
...
 

##run example

./ieatgor file.target file.gor

the extracted line are returned to standard out. 

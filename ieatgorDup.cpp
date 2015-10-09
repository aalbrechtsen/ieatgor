////////////////////////////////////////////////////////////////////////////
//    g++ -O3 -o ieatgor ieatgor.cpp -lz
//    ./ieatgor targetRegion.txt file.gor
//     Anders Albrechtsen
//     input files needs to be sorted
//     works for all files with chr and positions as the two first tab seperated columns
//////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <iostream>
#include <string>
#include "zlib.h"
#define LENS 100000 //this is the max number of bytes perline, should make bigger

//who needs functions. everthing in the main!!!!
int main(int argc, char **argv){
  int skip=0; // numer of lines to skip (e.g. to keep a header)
  int num=0;// is the file in numeric (1) or lexical (0) order 
  char *saveptrInput, *saveptrTar;
  int chrColumn=1;
  int posColumn=2;
  if(argc==1){
    fprintf(stderr,"\nRUN:\tieatgor <targetFile> <file> OPTIONS\n\n");
    fprintf(stderr,"target file format:\t chrName:start-end\n\n");
    fprintf(stderr,"OPTIONS:\n");
    fprintf(stderr,"\t-offset INT\t offset the target positions by INT\n");
    fprintf(stderr,"\t-skip INT\t number of lines to skip (will be printet)\n");
    fprintf(stderr,"\t-num INT\t are the chromosome in lexical (default 0) or numeric (1) order \n");
   fprintf(stderr,"\t-chr INT\t column of chr (default 1) \n");
    exit(0);
  }
  const char* targetFileName=argv[1];
  const char* inputFileName=argv[2];
  const char *delimsTarget = "\t-:\n";
  const char *delimsInput = "\t\n";
  FILE *targetFile = NULL;
  gzFile inputFile = NULL;
  int offset=0;
  if(NULL==(targetFile=fopen(targetFileName,"r"))){
    fprintf(stderr,"Error opening file: %s\n",targetFileName);
    exit(0);
  }
  if(NULL==(inputFile=gzopen(inputFileName,"r"))){
    fprintf(stderr,"Error opening file: %s\n",inputFileName);
    exit(0);
  }
  
  for(int i=3;i<argc;i++){
    if(strcmp(argv[i],"-offset")==0){
      i++;
      offset=atoi(argv[i]);
    }
    else if(strcmp(argv[i],"-skip")==0){
      i++;
      skip=atoi(argv[i]);
    }
    else if(strcmp(argv[i],"-num")==0){
      i++;
      num=atoi(argv[i]);
    }
   else if(strcmp(argv[i],"-chr")==0){
      i++;
      chrColumn=atoi(argv[i]);
    }
   else if(strcmp(argv[i],"-pos")==0){
      i++;
      posColumn=atoi(argv[i]);
    }
    else {
      printf("\tUnknown arguments: %s\n",argv[i]);
      printf("USE -offset AFTER the target and file\n");
      return 0;
    }

  }

  if(posColumn==chrColumn){
    printf("pos and chr column cannot be the same\n");
    return 0;
 

  }
  int mmax=posColumn;
  if(chrColumn>posColumn)
    mmax=chrColumn;
  char bufTarget[LENS];
  char bufInput[LENS];

  if(fgets(bufTarget,LENS,targetFile)==0)
    fprintf(stderr,"no data in target:\n");

  char *tok = strtok_r(bufTarget,delimsTarget,&saveptrTar);//strtok_r(bufTarget,delimsTarget,&saveptrTar);
  int chrTarInt;
  char* chrTarChar;
  
  if(num)
    chrTarInt=atoi(tok);
  else
    chrTarChar=strdup(tok); //not really need strdub in this case
  
  int start=atoi(strtok_r(NULL,delimsTarget,&saveptrTar));
  int stop=atoi(strtok_r(NULL,delimsTarget,&saveptrTar));
  char *f;
  
  //skip the first "skip" lines but print them
  for(int i=0;i<skip;i++){
    f = gzgets(inputFile,bufInput,LENS);
    char *tokSkip=strtok_r(bufInput,"\n",&saveptrInput);
    fprintf(stdout,"%s\n",bufInput);
    f=gzgets(inputFile,bufInput,LENS);
  } 


  if(tok==NULL){
    fprintf(stderr,"NULL pointer:\n");
    return 0;
  }
  

  if(num){//if the chromosones are in numeric order
    char* copy=NULL;
    int chrInput=0;
    int posInput=0;
    
    while((f=gzgets(inputFile,bufInput,LENS))) {
      free(copy);
      char *copy = strdup(bufInput);
      char *tok2=strtok_r(bufInput,delimsInput,&saveptrInput);
      for(int nColumn=1;nColumn<=mmax;nColumn++){
	if(nColumn==chrColumn){  
	  chrInput=atoi(tok2);
	}
	else if(nColumn==posColumn){
	  posInput=atoi(tok2);
	}
	tok2 = strtok_r(NULL,delimsInput,&saveptrInput);
      }

      //  fprintf(stderr,"chr %d %d %d\n",chrInput,chrTarInt,posInput);
      int comp=chrInput-chrTarInt;
      if(comp<0)
	continue;
      
      if(comp==0&&posInput<start)
	continue;
      while(comp>0||(comp==0&&posInput>stop)){
	if(fgets(bufTarget,LENS,targetFile)==0){
	  fprintf(stderr,"no more target file \n");
	  goto gotoEndOfLoopNum;
	  
	}
	
	chrTarInt=atoi(strtok_r(bufTarget,delimsTarget,&saveptrTar)); 
	start=atoi(strtok_r(NULL,delimsTarget,&saveptrTar));
	stop=atoi(strtok_r(NULL,delimsTarget,&saveptrTar));
	comp=chrInput-chrTarInt;
	//  fprintf(stdout,"all %s %d %d\n",chrTarInt,start,stop);
      }
      if(comp<0||posInput<start)
	continue;
      else{
	// fprintf(stdout,"realTar %s %d %d\n",chrTarInt,start,stop);
	//	fprintf(stdout,"%d\t%d\t%s",chrInput,posInput,bufInput+strlen(tok2)+strlen(tok2Chr)+2);
	fprintf(stdout,"%s",copy);
      }
    }
  gotoEndOfLoopNum:;
  }
  else{//begin lexical
      char* copy=NULL;
  
      char* chrInput=NULL;
      int posInput=0;
   
    while((f=gzgets(inputFile,bufInput,LENS))) {
      free(copy);
      char *copy = strdup(bufInput);   
      free(chrInput);


      char *tok2=strtok_r(bufInput,delimsInput,&saveptrInput);
      for(int nColumn=1;nColumn<=mmax;nColumn++){
	if(nColumn==chrColumn){  
	  chrInput=strdup(tok2);
	}
	else if(nColumn==posColumn){
	  posInput=atoi(tok2);
	}
	tok2 = strtok_r(NULL,delimsInput,&saveptrInput);
      }

 


      //     chrInput=strdup(strtok_r(bufInput,delimsInput,&saveptrInput));
      int comp=strcmp(chrInput,chrTarChar);
      if(comp<0)
	continue;
      
      
      //     char *tok2 = strtok_r(NULL,delimsInput,&saveptrInput);
      //     int posInput=atoi(tok2);
      
      if(comp==0&&posInput<start)
	continue;
      while(comp>0||(comp==0&&posInput>stop)){
	if(fgets(bufTarget,LENS,targetFile)==0){
	  fprintf(stderr,"no more target file \n");
	  goto gotoEndOfLoop;
	  
	}
	free(chrTarChar);
	chrTarChar=strdup(strtok_r(bufTarget,delimsTarget,&saveptrTar)); 
	start=atoi(strtok_r(NULL,delimsTarget,&saveptrTar))+offset;
	stop=atoi(strtok_r(NULL,delimsTarget,&saveptrTar))+offset;
	comp=strcmp(chrInput,chrTarChar);
	//  fprintf(stdout,"all %s %d %d\n",chrTarChar,start,stop);
      }
      if(comp<0||posInput<start)
	continue;
      else{
	// fprintf(stdout,"realTar %s %d %d\n",chrTarChar,start,stop);
	//fprintf(stdout,"%s\t%d\t%s",chrInput,posInput,bufInput+strlen(chrInput)+strlen(tok2)+2);
	fprintf(stdout,"%s",copy);
      }
    }
  gotoEndOfLoop:;
    free(chrTarChar);
    free(chrInput);
    
  }// end lexical
  
  fclose(targetFile);
  gzclose(inputFile);
  
  
  return 0;
}



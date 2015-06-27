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
  char *saveptrMaf, *saveptrTar;
  if(argc==1){
    fprintf(stderr,"\nRUN:\tieatgor <targetFile> <file> OPTIONS\n\n");
    fprintf(stderr,"target file format:\t chrName:start-end\n\n");
    fprintf(stderr,"OPTIONS:\n");
    fprintf(stderr,"\t-offset INT\t offset the target positions by INT\n");
    fprintf(stderr,"\t-skip INT\t number of lines to skip (will be printet)\n");
    fprintf(stderr,"\t-num INT\t are the chromosome in lexical (default 0) or numeric (1) order \n");
    exit(0);
  }
  const char* targetFileName=argv[1];
  const char* mafFileName=argv[2];
  const char *delimsTarget = "\t-:\n";
  const char *delimsMafs = "\t\n";
  FILE *targetFile = NULL;
  gzFile mafFile = NULL;
  int offset=0;
  if(NULL==(targetFile=fopen(targetFileName,"r"))){
    fprintf(stderr,"Error opening file: %s\n",targetFileName);
    exit(0);
  }
  if(NULL==(mafFile=gzopen(mafFileName,"r"))){
    fprintf(stderr,"Error opening file: %s\n",mafFileName);
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
    else {
      printf("\tUnknown arguments: %s\n",argv[i]);
      printf("USE -offset AFTER the target and file\n");
      return 0;
    }

  }
  char bufTarget[LENS];
  char bufMaf[LENS];

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
    f = gzgets(mafFile,bufMaf,LENS);
    char *tokSkip=strtok_r(bufMaf,"\n",&saveptrMaf);
    fprintf(stdout,"%s\n",bufMaf);
    f=gzgets(mafFile,bufMaf,LENS);
  } 


  if(tok==NULL){
    fprintf(stderr,"NULL pointer:\n");
    return 0;
  }
  

  if(num){//if the chromosones are in numeric order
    
    int chrMaf=0;
    while((f=gzgets(mafFile,bufMaf,LENS))) {
      char *tok2Chr=strtok_r(bufMaf,delimsMafs,&saveptrMaf);
      
      chrMaf=atoi(tok2Chr);
      // fprintf(stderr,"chr %d %d\n",chrMaf,chrTarInt);
      int comp=chrMaf-chrTarInt;
      if(comp<0)
	continue;
      char *tok2 = strtok_r(NULL,delimsMafs,&saveptrMaf);
      int pos=atoi(tok2);
      
      if(comp==0&&pos<start)
	continue;
      while(comp>0||(comp==0&&pos>stop)){
	if(fgets(bufTarget,LENS,targetFile)==0){
	  fprintf(stderr,"no more target file \n");
	  goto gotoEndOfLoopNum;
	  
	}
	
	chrTarInt=atoi(strtok_r(bufTarget,delimsTarget,&saveptrTar)); 
	start=atoi(strtok_r(NULL,delimsTarget,&saveptrTar));
	stop=atoi(strtok_r(NULL,delimsTarget,&saveptrTar));
	comp=chrMaf-chrTarInt;
	//  fprintf(stdout,"all %s %d %d\n",chrTarInt,start,stop);
      }
      if(comp<0||pos<start)
	continue;
      else{
	// fprintf(stdout,"realTar %s %d %d\n",chrTarInt,start,stop);
	fprintf(stdout,"%d\t%d\t%s",chrMaf,pos,bufMaf+strlen(tok2)+strlen(tok2Chr)+2);
      }
    }
  gotoEndOfLoopNum:;
  }
  else{//begin lexical
    
    char* chrMaf=NULL;
    
    while((f=gzgets(mafFile,bufMaf,LENS))) {
      free(chrMaf);
      chrMaf=strdup(strtok_r(bufMaf,delimsMafs,&saveptrMaf));
      int comp=strcmp(chrMaf,chrTarChar);
      if(comp<0)
	continue;
      
      
      char *tok2 = strtok_r(NULL,delimsMafs,&saveptrMaf);
      int pos=atoi(tok2);
      
      if(comp==0&&pos<start)
	continue;
      while(comp>0||(comp==0&&pos>stop)){
	if(fgets(bufTarget,LENS,targetFile)==0){
	  fprintf(stderr,"no more target file \n");
	  goto gotoEndOfLoop;
	  
	}
	free(chrTarChar);
	chrTarChar=strdup(strtok_r(bufTarget,delimsTarget,&saveptrTar)); 
	start=atoi(strtok_r(NULL,delimsTarget,&saveptrTar))+offset;
	stop=atoi(strtok_r(NULL,delimsTarget,&saveptrTar))+offset;
	comp=strcmp(chrMaf,chrTarChar);
	//  fprintf(stdout,"all %s %d %d\n",chrTarChar,start,stop);
      }
      if(comp<0||pos<start)
	continue;
      else{
	// fprintf(stdout,"realTar %s %d %d\n",chrTarChar,start,stop);
	fprintf(stdout,"%s\t%d\t%s",chrMaf,pos,bufMaf+strlen(chrMaf)+strlen(tok2)+2);
      }
    }
  gotoEndOfLoop:;
    free(chrTarChar);
    free(chrMaf);
    
  }// end lexical
  
  fclose(targetFile);
  gzclose(mafFile);
  
  
  return 0;
}



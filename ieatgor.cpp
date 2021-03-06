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



  int nKeys=0;
  int nLines=0;
  int skip=0; // numer of lines to skip (e.g. to keep a header)
  int num=0;// is the file in numeric (1) or lexical (0) order 
  char *saveptrMaf, *saveptrTar;
  if(argc==1){
    fprintf(stderr,"\nRUN:\tieatgor <targetFile> <file> OPTIONS\n\n");
    fprintf(stderr,"target file format:\t chrName:start-end\n or \t\t\t chrName:position\n\n");
    fprintf(stderr,"OPTIONS:\n");
    fprintf(stderr,"\t-offset INT\t offset the target positions by INT\n");
    fprintf(stderr,"\t-skip INT\t number of lines to skip (will be printet)\n");
    fprintf(stderr,"\t-num INT\t are the chromosome in lexical (default 0) or numeric (1) order \n");
    fprintf(stderr,"\t-rmChrTarget INT\t ignore \"chr\" in target \n");
    fprintf(stderr,"\t-rmChrInput INT\t ignore \"chr\" in input \n");
    exit(0);
  }
  const char* targetFileName=argv[1];
  const char* mafFileName=argv[2];
  const char *delimsTarget = "\t-:\n ";
  const char *delimsMafs = "\t\n ";
  FILE *targetFile = NULL;
  gzFile mafFile = NULL;
  int offset=0;
  int rmChrT=0;
  int rmChrI=0;
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
    else if(strcmp(argv[i],"-rmChrTarget")==0){
      i++;
      rmChrT=atoi(argv[i]);
    }
    else if(strcmp(argv[i],"-rmChrInput")==0){
      i++;
      rmChrI=atoi(argv[i]);
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

  char *tok;// = strtok_r(bufTarget,delimsTarget,&saveptrTar);//strtok_r(bufTarget,delimsTarget,&saveptrTar);
  int chrTarInt;
  char* chrTarChar;
       
 
     
  if(rmChrT){
    char *Tempp=strtok_r(bufTarget,"r",&saveptrTar); 
    tok=strtok_r(NULL,delimsTarget,&saveptrTar); 
  }
  else 
    tok = strtok_r(bufTarget,delimsTarget,&saveptrTar);
  
  if(num){
    chrTarInt=atoi(tok);
  }
  else{
    chrTarChar=strdup(tok); //not really need strdub in this case
    

  }
  int stop;
  int start=atoi(strtok_r(NULL,delimsTarget,&saveptrTar));
  char *pch = strtok_r(NULL,delimsTarget,&saveptrTar);
  if(pch == NULL)
    stop=start;
  else
  stop=atoi(pch);



  char *f;
  
  //skip the first "skip" lines but print them
  for(int i=0;i<skip;i++){
    f = gzgets(mafFile,bufMaf,LENS);
    char *tokSkip=strtok_r(bufMaf,"\n",&saveptrMaf);
    fprintf(stdout,"%s\n",bufMaf);
    // f=gzgets(mafFile,bufMaf,LENS);
  } 


  if(tok==NULL){
    fprintf(stderr,"NULL pointer:\n");
    return 0;
  }
  

  if(num){//if the chromosones are in numeric order
    int newKey=1;
    int chrMaf=0;
    char *tok2Chr;
    while((f=gzgets(mafFile,bufMaf,LENS))) {
      if(rmChrI){
	char *tok2ChrT=strtok_r(bufMaf,"r",&saveptrMaf);
	tok2Chr=strtok_r(NULL,delimsMafs,&saveptrMaf);
      }
      else
	tok2Chr=strtok_r(bufMaf,delimsMafs,&saveptrMaf);
      //fprintf(stdout,"all %s\n",tok2Chr);
    
      chrMaf=atoi(tok2Chr);
      //fprintf(stderr,"chr %d %d\n",chrMaf,chrTarInt);
      int comp=chrMaf-chrTarInt;
      if(comp<0)
	continue;
      char *tok2 = strtok_r(NULL,delimsMafs,&saveptrMaf);
      
      int pos=atoi(tok2);
      
      if(comp==0&&pos<start)
	continue;
      while(comp>0||(comp==0&&pos>stop)){
	newKey=1;
	if(fgets(bufTarget,LENS,targetFile)==0){
	  fprintf(stderr,"no more target file \n");
	  goto gotoEndOfLoopNum;
	  
	}
	if(rmChrT){
	  char *Tempp=strtok_r(bufTarget,"r",&saveptrTar); 
	  chrTarInt=atoi(strtok_r(NULL,delimsTarget,&saveptrTar)); 
	}
	else 
	  chrTarInt=atoi(strtok_r(bufTarget,delimsTarget,&saveptrTar)); 
	start=atoi(strtok_r(NULL,delimsTarget,&saveptrTar))+offset;
	char *pch = strtok_r(NULL,delimsTarget,&saveptrTar);
	if(pch == NULL)
	  stop=start;
	else
	  stop=atoi(pch)+offset;

	comp=chrMaf-chrTarInt;
	//fprintf(stdout,"all %s %d %d\n",chrTarInt,start,stop);
      }
      if(comp<0||pos<start)
	continue;
      else{
	nLines++;
	if(newKey)
	  nKeys++;
	// fprintf(stdout,"realTar %s %d %d\n",chrTarInt,start,stop);
	if(rmChrI==0)
	  fprintf(stdout,"%d\t%d\t%s",chrMaf,pos,bufMaf+strlen(tok2)+strlen(tok2Chr)+2);
	else
	  fprintf(stdout,"chr%d\t%d\t%s",chrMaf,pos,bufMaf+strlen(tok2)+strlen(tok2Chr)+2+3);
      }
      newKey=0;
    }
  gotoEndOfLoopNum:;
  }
  else{//begin lexical
      int newKey=1;
  
    char* chrMaf=NULL;
    
    while((f=gzgets(mafFile,bufMaf,LENS))) {

      free(chrMaf);

      
      //	char *tok2ChrT=strtok_r(bufMaf,"r",&saveptrMaf);
      //	tok2Chr=strtok_r(NULL,delimsMafs,&saveptrMaf);
	
      if(rmChrI){
	char *tok2ChrT=strdup(strtok_r(bufMaf,"r",&saveptrMaf));
	chrMaf=strdup(strtok_r(NULL,delimsMafs,&saveptrMaf));
	//fprintf(stdout,"chr %s\n",chrMaf);
      }
      else
	chrMaf=strdup(strtok_r(bufMaf,delimsMafs,&saveptrMaf));

      //fprintf(stdout,"chr %s\n",chrMaf);

      int comp=strcmp(chrMaf,chrTarChar);
      if(comp<0)
	continue;
    
      char *tok2 = strtok_r(NULL,delimsMafs,&saveptrMaf);
      int pos=atoi(tok2);
      
      if(comp==0&&pos<start)
	continue;
      while(comp>0||(comp==0&&pos>stop)){
		newKey=1;

	if(fgets(bufTarget,LENS,targetFile)==0){
	  fprintf(stderr,"no more target file \n");
	  goto gotoEndOfLoop;
	  
	}
	free(chrTarChar);

	if(rmChrT){
	  char *Tempp=strtok_r(bufTarget,"r",&saveptrTar); 
	  chrTarChar=strdup(strtok_r(NULL,delimsTarget,&saveptrTar)); 
	}
	else 
	  chrTarChar=strdup(strtok_r(bufTarget,delimsTarget,&saveptrTar)); 

	
	start=atoi(strtok_r(NULL,delimsTarget,&saveptrTar))+offset;
	char *pch = strtok_r(NULL,delimsTarget,&saveptrTar);
	if(pch == NULL)
	  stop=start;
	else
	  stop=atoi(pch)+offset;
	
	comp=strcmp(chrMaf,chrTarChar);
	//  fprintf(stdout,"all %s %d %d\n",chrTarChar,start,stop);
      }
      if(comp<0||pos<start)
	continue;
      else{
	nLines++;
	if(newKey)
	  nKeys++;
	// fprintf(stdout,"realTar %s %d %d\n",chrTarChar,start,stop);
	//	fprintf(stdout,"%s\t%d\t%s",chrMaf,pos,bufMaf+strlen(chrMaf)+strlen(tok2)+2);
	if(rmChrI==0)
	  fprintf(stdout,"%s\t%d\t%s",chrMaf,pos,bufMaf+strlen(chrMaf)+strlen(tok2)+2);
	else
	  fprintf(stdout,"chr%s\t%d\t%s",chrMaf,pos,bufMaf+strlen(chrMaf)+strlen(tok2)+2+3);
      }
      newKey=0;

    }
  gotoEndOfLoop:;
    free(chrTarChar);
    free(chrMaf);
    
  }// end lexical
  
  fclose(targetFile);
  gzclose(mafFile);
  
  fprintf(stderr,"Used keys=%d, \tLine match=%d\n",nKeys,nLines);
  return 0;
}



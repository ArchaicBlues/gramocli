/* GramoFile - Main

 * Copyright (C) 1998 J.A. Bezemer
 * Modifications Copyright (C) 2024 ArchaicBlues: removed interactive
 * ncurses UI, added non-interactive CLI argument interface (gramocli).
 *
 * Licensed under the terms of the GNU General Public License.
 * ABSOLUTELY NO WARRANTY.
 * See the file `COPYING' in this directory.
 */


#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "signpr_general.h"
#include "tracksplit.h"
#include "signpr_main.h"


signed int checkNum(char *num){
  if(!strcmp(num,"0") || !strcmp(num,"1") || !strcmp(num,"2") || !strcmp(num,"3") || !strcmp(num,"4") || \
  !strcmp(num,"5"))
  {
    signed int z = (signed int) *num - 0x30;
    return z;
  }
  return -1;
}

signed int getNum(int argc, char *argv[], char *p){
  for (int i=0; i<argc; i++){
    // printf("p=%s\n",p);
    // printf("argv[%d]=%s\n",i,argv[i]);
    if (strcmp(argv[i],p) == 0) return i+1;
  }
  return -1;
}

//call i.e. "source processwav -i infile.wav -f 1 4 6 "
int main (int argc, char* argv[])
{
  //if (argc == 1) exitStatus(STATUS_NO_INFILE);
  if ((argc < 2) || (argc==2 && !strcmp(argv[1],"-h"))) {
    printf("Usage: ./gramocli -i <infile.wav> -s <100..600> -f <filter1 filter2 ..> -o <out.wav>]\n \
    0 - simple median filter\n \
    1 - simple mean filter\n \
    2 - conditional median filter\n \
    3 - double median filter\n \
    4 - conditional median filter II\n \
    5 - RMS filter\n \
    gramocli splits <infile.wav> in <out1.wav>, <out2.wav> ...depending on track pauses\n \
    Each of out*.wav will be processed with the selected filters or leave it.\n \
    If no full path of <out.wav> given: out*.wav are stored in the current directory\n \
    Note <infile>:\n \
    - requires 'RIFF', PCM 44100Hz, 16-Bit, Stereo \
    - requires full path length\n \
    - For track location, do not record too much silence before first & after the last track.\n \
      3-5 seconds silence will get best results.\n \
    - do not record two sides of a LP record in a single sound file, make two files instead.\n \
    Example:\n \
    gramocli -i /home/buero/rec/infile.wav -f 0 4 -o /home/buero/processed/out.wav\n");

    if (argc > 1){
      if (!strcmp(argv[1],"-h")) return 0;
    }    exitStatus(STATUS_NO_INFILE);
  } 
  char startdir[180]={0};
  char infile[180]={0};
  char infilter[20]={0};
  char outfile[180]={0};
  int silence = 300;
  int split = 1;

  if (getcwd(startdir,sizeof (startdir)) == NULL){
    printf("URI too long\n");
    exitStatus(STATUS_STARTDIR_LENGTH);
  }
  printf("startdir: %s\n",startdir);
  printf("Anzahl der Argumente = %d\n", argc);
  for (unsigned char i=0;i<argc;i++){
    printf("Argument %d = %s\n",i,argv[i]);
  }

  int num = getNum(argc, argv, "-i");
  if (num < 0) {
    printf("Error, no <infile> no para given\n");
    exitStatus(STATUS_NO_INFILE);
  }

  if (argc > num && !strcmp(argv[num-1],"-i")){
    // strcat(infile,startdir);
    strcat(infile,argv[num]);
    printf("infile = %s\n", infile);
    if (access(infile,F_OK) == 0) { //if EXIST
      if (access(infile,4) == -1){ //if no READ persissions
        exitStatus(STATUS_INFILE_READ_PERMISSIONS);
      }
    } else {
      exitStatus(STATUS_INFILE_NOT_FOUND);
    }

    num = getNum(argc, argv, "-s");
    if (!num) exitStatus(ERROR_NO_PARA_FOUND);

    if (argc > num && !strcmp(argv[num-1],"-s")){
      printf("argv[%d]=%s\n",num,argv[num]);
      silence = atoi(argv[num]);
      if (!silence){
        printf("Error: bad silence num\n");
        exitStatus(ERROR_BAD_SILENCE_NUM);
      }//else
        //silence = 300;
    }
    printf("silence=%d\n",silence);


    //max 6 filter 
//    num = 6;
    num = getNum(argc, argv, "-f");
    if (num >= 0){
      number_of_filters=0;
      unsigned int z;
      if (argc > num && !strcmp(argv[num-1],"-f")){
        //printf("argv[%d]=%s\n",num,argv[num]);
        if ((z=(unsigned int)checkNum(argv[num])) == -1){
          printf("Error: bad filter num\n");
          exitStatus(STATUS_BAD_FILTER_NUM);
        }
        filter_type[number_of_filters++]=z+1;
        strcpy(infilter,argv[num++]);
        for (unsigned int i=5; i<10; i++){
          //printf("argv[%d]=%s\n",num,argv[num]);
          if (argc > num && strcmp(argv[num],"-t")){
            if ((z=(unsigned int)checkNum(argv[num])) == -1){
              printf("Error: bad filter num\n");
              exitStatus(STATUS_BAD_FILTER_NUM);
            }
            filter_type[number_of_filters++]=z+1;
            strcat(infilter," ");
            strcat(infilter,argv[num++]);
          } else {
            num++;
            break;
          }
        }
      }
    }
    printf("infilter = %s\n", infilter);
//    num++;
    num = getNum(argc, argv, "-t");
    if (num >= 0){
      if (argc > num && !strcmp(argv[num-1],"-t")){
        //printf("argv[%d]=%s\n",num,argv[num]);
        split = atoi(argv[num]);
        if (!split){
          printf("Error: bad split num\n");
          exitStatus(ERROR_BAD_SPLIT_NUM);
        }else
          split = 1;
      }
      printf("split=%d\n",split);
    }
  }
  else{
    printf("Error, no <infile> given\n");
    exitStatus(STATUS_NO_INFILE);
  }
  num = getNum(argc, argv, "-o");
  if (num <= 0) exitStatus(ERROR_NO_OUTFILE_FOUND);
//ls ad  printf("-o found at %d\n",num);

  if (argc > num && !strcmp(argv[num-1],"-o")){
    strcat(outfile,argv[num]);
  }
  else {
    strcat(outfile,startdir);
    strcat(outfile,"/song.wav");
  }
  printf("outfile = %s\n", outfile);

  if (argc > 14){
    printf("Warning: too many arguments\n");
  }

  num = getNum(argc, argv, "-tracks");
  num--;
  if (num < 0){
    tracksplit_main(startdir, infile, silence, split); 
  }
  signproc_main(startdir, infile, outfile);
  exitStatus(STATUS_OK);
}

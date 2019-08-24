#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef _MSC_VER
// #pragma warning(disable : 4756)
#include <direct.h> //mkdir
#define snprintf _snprintf
#endif

#ifndef _MSC_VER
#include <unistd.h>
#endif

#define TEST 2

#if TEST==1
#include "avilib.h"
#elif TEST==2
#include "kohn_avi.h"
#endif

#include "jo_jpeg.h"
#include "sobol.h"
#include "timer.h"
#include "tinydir.h"


#if defined(_MSC_VER)
#include <io.h>
#define F_OK 0x06
#endif

#define FPS 30


double minOf(double x, double y) {
  return (x>y)?y:x;
}

double maxOf(double x, double y) {
  return (x>y)?x:y;
}

double clamp(double x, double minVal, double maxVal) {
  return minOf(maxOf(x, minVal), maxVal);
}


void createDir(const char *n) {
#if defined(_WIN32)
  mkdir(n);
#elif defined(__linux__)
  mkdir(n, 777);
#endif
}

int lastFileNum(const char *folderName) {
  int num=-1;
  tinydir_dir dir;

  if(tinydir_open(&dir, folderName) != -1) {
    while(dir.has_next) {
      tinydir_file file;
      tinydir_readfile(&dir,&file);
      int fnLen=strlen(file.name);

      char numStr[256];
      numStr[0]='\0';
      memset(numStr,0,256);
      if(!file.is_dir) {
        int i,j=0;

        for(i=0;i<fnLen;i++) {

          // if(file.name[i]=='0' && j==0) {
          //   j++;
          // } else
          if(file.name[i]>='0' && file.name[i]<='9') {
            numStr[j++]=file.name[i];

          } else if(j>0) {
            break;
          }
        }

        if(numStr[0]!='\0') {
          // printf("%s\n",numStr);
          int num2=atoi(numStr);

          if(num2 > num) {
            num=num2;
          }
        }
      }

      tinydir_next(&dir);
    }

    tinydir_close(&dir);
  }

  return num;
}


unsigned char floatToByte(float f) {
  return (unsigned char)(minOf(1.0f,f)*255.0f);
}

int main() {

  double startTime;
  startTime=timer();

  sobol_state_t qrnd;
  sobol_init(&qrnd,2);

  int canvasWidth=640;
  int canvasHeight=480;
  int canvasSize=canvasWidth*canvasHeight;

  float *canvas;
  canvas=(float*)malloc(sizeof(float)*canvasSize*3);

  unsigned char *bitmap;
  bitmap=(unsigned char*)malloc(canvasSize*3);


#if TEST==1
  avi_t *avifile;
#elif TEST==2
  struct kavi_t *kavi;
  // unsigned char snd_buffer[22050];
  // struct kohn_avi_audio_t auds;

#endif


  createDir("output");
  createDir("output/tmp");
  int fileNum=lastFileNum("output")+1;

  char fn[512];
  snprintf(fn,sizeof(fn),"output/output%04i.avi",fileNum);

  if(access(fn, F_OK )!=-1) {
    fprintf(stderr,"'%s' already exists.\n",fn);

    return 1;
  }

  float frame;
  int count=0;

  for(frame=0.0f;frame<=15.0f;frame+=1.0f/(float)FPS) {

    //sobol_init(&qrnd,2);

    int i;

    //
    for(i=0;i<canvasSize;i++) {
      double v[2];
      sobol_get(&qrnd,2,v);
      // printf("%f %f\n",v[0],v[1]);

      v[0]=v[0]*2.0-1.0;
      v[1]=v[1]*2.0-1.0;
      float a[3];
      a[0]= (float)(clamp(v[0], -1.0, 1.0)*0.5 + 0.5);
      a[1]= (float)(clamp(v[1], -1.0, 1.0)*0.5 + 0.5);
      sobol_get(&qrnd,2,v);
      a[2]= (float)(clamp((v[0] + v[2] * 0.5), -1.0, 1.0)*0.5 + 0.5);
      // canvas[i*3]=v[0];
      // canvas[i*3+1]=v[1];
      // canvas[i*3+2]=(v[0]+v[1])*0.5;



      canvas[i*3]=a[0];
      canvas[i*3+1]=a[1];
      canvas[i*3+2]=a[2];
// printf("%f %f %f\n",canvas[i*3],canvas[i*3+1],canvas[i*3+2]);
// printf("%f %f %f\n",a[0],a[1],a[2]);
    }

    //
    for(i=0;i<canvasSize;i++) {
      bitmap[i*3]=floatToByte(canvas[i*3]);
      bitmap[i*3+1]=floatToByte(canvas[i*3+1]);
      bitmap[i*3+2]=floatToByte(canvas[i*3+2]);
    }



    char fn2[256];
    snprintf(fn2,sizeof(fn2),"output/tmp/tmp%04i_%i.jpg",fileNum,count);
    jo_write_jpg(fn2,bitmap,canvasWidth,canvasHeight,3, 100);

    printf("frame %g\n",frame);

    count++;
    //



  }

  //
  int i;


#if TEST==1

  avifile = AVI_open_output_file(fn);


  if(!avifile) {
    printf("aa\n");
    return 1;
  }



  AVI_set_video(avifile,canvasWidth,canvasHeight,(double)FPS,"MJPG");
#elif TEST==2

  // Make some stupid sound
  // for (i=0; i<22050; i++) {
  //     double v[2];
  //     sobol_get(&qrnd,2,v);
  //     double s=(log((double)i*350.0 )*10.0+50.0 )/255.0;
  //     snd_buffer[i]= (unsigned char)s;

  //     // snd_buffer[i]=rand()%255;//i&0xff;
  // }

  // auds.channels=1;
  // auds.bits=8;
  // auds.samples_per_second=22050;

  kavi=kavi_open(fn,canvasWidth,canvasHeight,"MJPG",FPS,NULL/*&auds*/);

  if (kavi==0) {
    printf("Couldn't open AVI for writing\n");
    return 1;
  }
#endif


  for(i=0;i<count;i++) {

    char fn2[256];
    snprintf(fn2,sizeof(fn2),"output/tmp/tmp%04i_%i.jpg",fileNum,i);

    FILE *in;
    unsigned char *buffer=0;
    int buffer_len=0;

    in=fopen(fn2,"rb");

    if (in==0) {
      printf("Cannot open test.jpeg %s\n",fn2);
      return 1;
    }

    fseek(in, 0, SEEK_END);
    buffer_len=(int)ftell(in);
    fseek(in,0,SEEK_SET);
    buffer=(unsigned char *)malloc(buffer_len);
    fread(buffer,1,buffer_len,in);
    fclose(in);


#if TEST==1
    // add_avifile_jpeg_frame( avifile,  bitmap, CANVAS_WIDTH,CANVAS_HEIGHT);
    AVI_write_frame(avifile,(char*)buffer,buffer_len,1);
#elif TEST==2
    kavi_add_frame(kavi,buffer,buffer_len);
    // if ((i+1)%30==0) {
    //   kavi_add_audio(kavi,snd_buffer,22050);
    // }

    if ((i%100)==0) {
      printf("%d frames so far...\n",i);
    }


#endif
    free(buffer);
  }


#if TEST==1
  AVI_close(avifile);
#elif TEST==2
kavi_close(kavi);
#endif


  //

  printf("Wrote '%s'.\n",fn);


 printf("Output took %g seconds.\n",timer()-startTime);

#ifdef _MSC_VER
  system("pause");
#endif

  return 0;
}

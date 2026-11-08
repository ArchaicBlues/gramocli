/* General functions for signal processing - Header

 * Copyright (C) 1998 J.A. Bezemer
 *
 * Licensed under the terms of the GNU General Public License.
 * ABSOLUTELY NO WARRANTY.
 * See the file `COPYING' in this directory.
 */

#ifndef HAVE_SIGNPR_GENERAL_H
#define HAVE_SIGNPR_GENERAL_H




//status.tab index
#define STATUS_OK 0
#define STATUS_STARTDIR_LENGTH 1
#define STATUS_BAD_FILTER_NUM 2
#define STATUS_NO_INFILE 3
#define STATUS_INFILE_READ_PERMISSIONS 4
#define STATUS_INFILE_NOT_FOUND 5
#define ERROR_TRACKS_NOT_FOUND 6
#define ERROR_NUMBER_OF_TRACKS_NOT_FOUND 7
#define ERROR_WRONG_NUMBER_OF_TRACKS 8
#define ERROR_EXPERIMET_FILTER 9
#define ERROR_TRACKXXStart_NOT_FOUND 10
#define ERROR_TRACKXXEnd_NOT_FOUND 11
#define ERROR_CONVERT_START_TIME 12
#define ERROR_CONVERT_END_TIME 13
#define ERROR_UNSUITABLE_TRACK_INFO 14
#define ERROR_TRACKS_POS_NOT_FOUND 15
#define ERROR_OPEN_DESTINATION_FILE 16
#define ERROR_OPEN_INPUT_WAV_FILE 17
#define ERROR_WAV_FILE_TOO_SHORT 18
#define ERROR_SOURCE_FILE_NOT_WAV_TYPE 19
#define ERROR_INPUT_WAV_FREQUENCY_NOT_44100Hz 20
#define ERROR_WAV_FORMAT 21
#define ERROR_WAV_NOT_STEREO 22
#define ERROR_WAV_16BIT 23
#define ERROR_OPEN_INFILE 24
#define ERROR_WRITE_TRACKS_FILE 25
#define ERROR_SOURCE_FILE_PROBLEM 26
#define ERROR_BAD_SILENCE_NUM 27
#define ERROR_BAD_SPLIT_NUM 28
#define ERROR_NO_PARA_FOUND 29
#define ERROR_NO_OUTFILE_FOUND 30

typedef struct
  {
    char **items;
    int number;
    int y;
    int x;			/* (y,x): upper left corner */
    int h;
    int w;			/* h=height, w=width */
    int selected;
    int firstonscreen;
    int last_of_1st_part;	/* 'private' for dirfile-menu */
    int hasfocus;
  }
scrollmenu_t;



/* SAMPLES */

typedef struct
  {
    signed short left;
    signed short right;
  }
sample_t;

typedef struct
  {
    signed short leftmin;
    signed short rightmin;
    signed short leftmax;
    signed short rightmax;
  }
max_sample_t;

typedef struct
  {
    signed long left;
    signed long right;
  }
longsample_t;

typedef struct
  {
    double left;
    double right;
  }
doublesample_t;


/* BUFFER */

typedef struct
  {
    sample_t *array;
    long currpos;
    long arraylength;
    long pre_length;		/* "read ahead" */
    long post_length;		/* "remember" */
#ifdef TURBO_BUFFER
    int *indextable;
#endif

  }
buffer_t;


/* PARAM */

typedef struct
  {
    buffer_t buffer;
    buffer_t buffer2;
    buffer_t buffer3;
    buffer_t buffer4;
    int filterno;		/* 'serial number' of filter, 
				   0=read_from_disk */
    long postlength1, prelength1, postlength2, prelength2, postlength3,
      prelength3, postlength4, prelength4;
    signed short *sslist1;
    signed short *sslist2;

    int int1;
    long long1;
    long long2;
  }
param_t;

typedef param_t *parampointer_t;


/* FILTER DATA */

#define MAX_FILTERS  50

extern parampointer_t parampointerarray[MAX_FILTERS];
extern int filter_type[MAX_FILTERS];
extern int number_of_filters;
extern max_sample_t maxsample;
//extern struct state statuskey[];

/* other */
signed int checkNum(char *num);
void exitStatus(unsigned char index);


/* BASIC SCREEN I/O */

void write_sample_to_screen (sample_t data);

sample_t read_from_keyboard ();


/* BUFFER FILL FUNCTION */

typedef sample_t (*fillfuncpointer_t) (long offset, long offset_zero,
				       parampointer_t parampointer);


/* BUFFER FUNCTIONS */

buffer_t init_buffer (long post_length, long pre_length);

void delete_buffer (buffer_t * buffer);

#ifdef TURBO_BUFFER

#define get_from_buffer(buffer,offset) (buffer)->array[(buffer)->indextable[(buffer)->currpos+offset]]

#define put_in_buffer(buffer,offset,sample) (buffer)->array[(buffer)->indextable[(buffer)->currpos+offset]]=sample

#else /* if not TURBO_BUFFER */

sample_t get_from_buffer (buffer_t * buffer, long offset);

void put_in_buffer (buffer_t * buffer, long offset, sample_t sample);

#endif /* (not) TURBO_BUFFER */

void advance_current_pos (buffer_t * buffer, int filterno);

void advance_current_pos_custom (buffer_t * buffer, fillfuncpointer_t fillfunc,
			     long offset_zero, parampointer_t parampointer);


/* QUICK SORT */

/* One for signed shorts, max size 32676 */

void qsort2 (signed short *a, int n);	/* a: pointer to start of array      */
					/* n: # elements in array            */

/* And one for doubles, max size 2G */

void qsort2double (double *a, long n);	/* a: pointer to start of array      */
					/* n: # elements in array            */


/* MEDIAN */

signed short median (signed short *a, int n);


/* BUILDING THE LIST OF FILTERS */

void add_to_filterlist (scrollmenu_t * filtlist,int *filtnumbers,
			char **helptexts, int filternumber, char *filtername,
			char *helptext);

void make_filterlist (scrollmenu_t * filtlist, int *filtnumbers, char **helptexts);


/* GET SAMPLE FROM FILTER */

sample_t get_sample_from_filter (int filterno);


/* INIT & DELETE FILTERS */

void init_filters ();

void delete_filters ();


/* PARAM DEFAULTS */

void param_defaults (parampointer_t parampointer, int filtertype);


/* PARAM SCREENS */

void param_screen (parampointer_t parampointer, int filtertype);


/* FILTER NUMBERS ETC. */

#define SIMPLE_MEDIAN_FILTER	1
#define SIMPLE_MEDIAN_NAME	"Simple Median Filter"
#define SIMPLE_MEDIAN_HELPTEXT  \
"Interpolate short ticks."

#define SIMPLE_MEAN_FILTER	2
#define SIMPLE_MEAN_NAME	"Simple Mean Filter"
#define SIMPLE_MEAN_HELPTEXT	\
"'Smooth' the signal by taking the mean of samples."

#define COND_MEDIAN_FILTER	3
#define COND_MEDIAN_NAME	"Conditional Median Filter"
#define COND_MEDIAN_HELPTEXT	\
"Remove ticks while not changing rest of signal."

#define DOUBLE_MEDIAN_FILTER	4
#define DOUBLE_MEDIAN_NAME	"Double Median Filter"
#define DOUBLE_MEDIAN_HELPTEXT	\
"Interpolate short ticks and correct interpolations."

#define COND_MEDIAN2_FILTER	5
#define COND_MEDIAN2_NAME	"Conditional Median Filter II"
#define COND_MEDIAN2_HELPTEXT	\
"Remove ticks while not changing rest of signal - Better."

#define RMS_FILTER		6
#define RMS_NAME		"RMS Filter"
#define RMS_HELPTEXT		\
"Compute the `running' Root-Mean-Square of the signal."

//normalize amplitude!
#define COPYONLY_FILTER		7 
#define COPYONLY_NAME		"Copy Only"
#define COPYONLY_HELPTEXT	\
"normalize amplitude"

#define MONOIZE_FILTER          8
#define MONOIZE_NAME            "Convert to mono"
#define MONOIZE_HELPTEXT        \
"Average left & right signals."

#define EXPERIMENT_FILTER	9
#define EXPERIMENT_NAME		"Experimenting Filter"
#define EXPERIMENT_HELPTEXT	\
"The filter YOU are experimenting with (in signpr_exper.c)"

#define TRUE 1
#define FALSE 0

#endif /* HAVE_SIGNPR_GENERAL_H */

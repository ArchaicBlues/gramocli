//"all.wav.tracks" siehe am Ende dieses File

/* Signal Processing Main

 * Copyright (C) 1998 J.A. Bezemer
 * Modifications Copyright (C) 2024 ArchaicBlues: removed interactive
 * ncurses UI, added non-interactive CLI argument interface (gramocli).
 *
 * Licensed under the terms of the GNU General Public License.
 * ABSOLUTELY NO WARRANTY.
 * See the file `COPYING' in this directory.
 */

#include "signpr_general.h"
#include "signpr_main.h"

#include "signpr_wav.h"
#include "fmtheaders.h"
#include "secshms.h"
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// #ifndef OLD_CURSES
// #include <ncurses.h>
// #else
// #include <curses.h>
// #endif


int
signproc_get_options (char *startdir, char *infilename, char *outfilename, int *filtnumbers,
		      char **helptexts, scrollmenu_t *selectedfilts,
		      int *usetracktimes, int *usebeginendtime,
		      double *begintime, double *endtime)
{
  char oldinfilename[250];
  struct stat buf;

  oldinfilename[0] = '\0';

  if (strcmp (infilename, oldinfilename))
	{
	    strcpy (oldinfilename, infilename);
	    stat (infilename, &buf);
	    if (buf.st_size < sizeof (wavhead))
	      *endtime = 0;
	    else
	      *endtime = (buf.st_size - sizeof (wavhead))	/ (2 * 2 * 44100.);
	    *begintime = 0;
  }
  //------------------------------
  // //Example:
	// selectedfilts->items[0] = "7"; // cond.Filter II
	// selectedfilts->items[1] = "2"; // simple median Filter
	// selectedfilts->items[2] = "3"; // simple mean Filter
  // number_of_filters=3;
  // filter_type[0]=COND_MEDIAN2_FILTER;
  // filter_type[1]=SIMPLE_MEDIAN_FILTER;
  // filter_type[2]=SIMPLE_MEAN_FILTER;
    
  //------------------------------
  
	*usetracktimes = 1;
	*usebeginendtime = 0;
  return 1;
}

long totalsize_samples;
long current_total_sample;
long tracksize_samples;
long current_sample;

#define TRACKS_TEXT		"[Tracks]"
#define NUM_OF_TRACKS_TEXT	"Number_of_tracks="

#define MORE_VERBOSE

int
load_track_times (char *filename, beginendsample_t * tracktimes,
		  int *number_of_tracks)
/* Returns: 0: error, 1: OK */
{
  char tempstring[250];
  char tempstring2[250];
  FILE *tracksfile;
  long tracks_pos;
  int i;
  double seconds;

  strcpy (tempstring, filename);
  strcat (tempstring, ".tracks");

  tracksfile = fopen (tempstring, "r");

  if (tracksfile == NULL)
    return 0;

  tempstring[0] = '\0';
  while (!feof (tracksfile) &&
	 strncasecmp (tempstring, TRACKS_TEXT, strlen (TRACKS_TEXT)))
    fgets (tempstring, 250, tracksfile);

  if (feof (tracksfile))	/* [Tracks] on last line => stop reading */
    {
#ifdef MORE_VERBOSE
      exitStatus(ERROR_TRACKS_NOT_FOUND);

#endif
      fclose (tracksfile);
      return 0;
    }

  tracks_pos = ftell (tracksfile);

  tempstring[0] = '\0';
  while (!feof (tracksfile) && strncasecmp (tempstring,
			   NUM_OF_TRACKS_TEXT, strlen (NUM_OF_TRACKS_TEXT)))
    fgets (tempstring, 250, tracksfile);

  if (strncasecmp (tempstring, NUM_OF_TRACKS_TEXT,
		   strlen (NUM_OF_TRACKS_TEXT)))
    {
#ifdef MORE_VERBOSE
      exitStatus(ERROR_NUMBER_OF_TRACKS_NOT_FOUND);
#endif
      fclose (tracksfile);
      return 0;
    }

  *number_of_tracks = atoi (tempstring + strlen (NUM_OF_TRACKS_TEXT));

  if (*number_of_tracks < 1 || *number_of_tracks > 99)
    {
#ifdef MORE_VERBOSE
      exitStatus(ERROR_WRONG_NUMBER_OF_TRACKS);
#endif
      fclose (tracksfile);
      return 0;
    }

  for (i = 1; i <= *number_of_tracks; i++)
    {
      sprintf (tempstring2, "Track%02dstart=", i);	/* Start time */
      fseek (tracksfile, tracks_pos, SEEK_SET);

      tempstring[0] = '\0';
      while (!feof (tracksfile) && strncasecmp (tempstring,
					 tempstring2, strlen (tempstring2)))
    	fgets (tempstring, 250, tracksfile);

      if (strncasecmp (tempstring, tempstring2, strlen (tempstring2)))
      {
    #ifdef MORE_VERBOSE
        exitStatus (ERROR_TRACKXXStart_NOT_FOUND);
        
    #endif
        fclose (tracksfile);
        return 0;
      }

      tempstring[strlen (tempstring) - 1] = '\0';	/* Del \n */
      if (!hmsf2fsec (tempstring + strlen (tempstring2), &seconds))
      {
    #ifdef MORE_VERBOSE
        exitStatus (ERROR_CONVERT_START_TIME);
    #endif
        fclose (tracksfile);
        return 0;
      }

      tracktimes[i].begin = 44100 * seconds;

      sprintf (tempstring2, "Track%02dend=", i);	/* End time */
      fseek (tracksfile, tracks_pos, SEEK_SET);

      tempstring[0] = '\0';
      while (!feof (tracksfile) && strncasecmp (tempstring,tempstring2, strlen (tempstring2)))
	    fgets (tempstring, 250, tracksfile);

      if (strncasecmp (tempstring, tempstring2, strlen (tempstring2)))
      {
#ifdef MORE_VERBOSE
        exitStatus (ERROR_TRACKXXEnd_NOT_FOUND);
#endif
        fclose (tracksfile);
        return 0;
      }

      tempstring[strlen (tempstring) - 1] = '\0';	/* Del \n */
      if (!hmsf2fsec (tempstring + strlen (tempstring2), &seconds))
      {
#ifdef MORE_VERBOSE
	  exitStatus (ERROR_CONVERT_END_TIME);
#endif
        fclose (tracksfile);
        return 0;
      }

      tracktimes[i].end = 44100 * seconds;
    }

  fclose (tracksfile);
  return 1;
}


void
signproc_main (char *startdir, char *infile, char *outfile)
{
  printf("start signproc_main..\n");
  // char infilename[250];
  // char outfilename[250];
  char baseoutfilename[250];
  char outfileextension[250];

  /* The menu settings are made static to allow the selected 
     filters and settings to be remembered between invocations */

  static int first_entry = 1;
  static scrollmenu_t selectedfilts;
  static scrollmenu_t filtlist;
  static char *filtlist_items[MAX_FILTERS + 10];
  static int filtnumbers[MAX_FILTERS + 10];
  static char *helptexts[MAX_FILTERS + 10];
  static char *selectedfilts_items[MAX_FILTERS];
  static int usebeginendtime = 0, usetracktimes = 1;
  static double begintime = 0, endtime = 0;

  struct stat buf;
  int i;
  char *charptr;
  //int in_ch;
  beginendsample_t tracktimes[100];	/* max. 99 tracks: 1 (!) - 99 */
  int number_of_tracks;

    if (first_entry)
    {
      filtlist.items = filtlist_items;	/* malloc also works :) */
      make_filterlist (&filtlist, filtnumbers, helptexts);

      selectedfilts.items = selectedfilts_items;
      for (unsigned int i=0; i<number_of_filters; i++)
      {
        // selectedfilts->items[0] = "7"; // cond.Filter II
        // selectedfilts->items[1] = "2"; // simple median Filter
        // selectedfilts->items[2] = "3"; // simple mean Filter
        selectedfilts.items[i] = filtlist.items[filter_type[i]+2];
        //filter_type[number_of_filters] = filtnumbers[7];
        parampointerarray[i] = (parampointer_t) malloc (sizeof (param_t));
        param_defaults (parampointerarray[i], filter_type[i]);
      }
      // selectedfilts.number = number_of_filters;
      // selectedfilts.selected = number_of_filters - 1;
/* ----- End (default filter) */

      first_entry = 0;
    }

  if (!signproc_get_options (startdir, infile, outfile,
			  filtnumbers, helptexts,  &selectedfilts,
		    &usetracktimes, &usebeginendtime, &begintime, &endtime))
    return;

  strcpy (baseoutfilename, outfile);
  strcpy (outfileextension, ".wav");

  /* IF there is a last '.' AND it's after
     the last '/' THEN make a new basename
     and extension */
  if ((charptr = strrchr (outfile, '.')) != NULL && strchr (charptr, '/') == NULL)
  {
    baseoutfilename[charptr - outfile] = '\0';
    strcpy (outfileextension, charptr);
  }

  if (usebeginendtime)
  {
    number_of_tracks = 1;
    tracktimes[1].begin = begintime * 44100;
    tracktimes[1].end = endtime * 44100;
  }
  else if (usetracktimes)
  {
    if (!load_track_times (infile, tracktimes, &number_of_tracks))
    {
      exitStatus (ERROR_UNSUITABLE_TRACK_INFO);
      return;
    }
  }
  else
    /* entire file */
    {
      number_of_tracks = 1;
      stat (infile, &buf);
      tracktimes[1].begin = 0;
      tracktimes[1].end = (buf.st_size - sizeof (wavhead)) / (2 * 2);
    }

  totalsize_samples = 0;	/* calculate sample totals */
  current_total_sample = 0;
  for (i = 1; i <= number_of_tracks; i++)
    totalsize_samples += tracktimes[i].end - tracktimes[i].begin;

  if (!openwavsource (infile))
    return;			/* open source */

  for (i = 1; i <= number_of_tracks; i++)
  {
    /* calculate #samples for this track */
    tracksize_samples = tracktimes[i].end - tracktimes[i].begin;

    /* seek to beginsample */
    if (!seeksamplesource (tracktimes[i].begin))
  	{
  	  exitStatus (ERROR_TRACKS_POS_NOT_FOUND);
	    break;
	  }

    if (number_of_tracks > 1)	
	    sprintf (outfile, "%s%02d%s", baseoutfilename, i, outfileextension);

      /* open destination file */
    if (!openwavdest (outfile, tracksize_samples * 4))
	  {
  	  exitStatus (ERROR_OPEN_DESTINATION_FILE);
	    break;
	  }

    init_filters ();

    /* process the signal */
    for (current_sample = 0; current_sample < tracksize_samples; current_sample++)
    {
      /* process one sample */
      writesampledest (get_sample_from_filter (number_of_filters - 1));
      current_total_sample++;	/* update total count */
    }

    // //normalize amplitude
    // seeksamplesource (tracktimes[i].begin);
    // maxsample.leftmin=0;
    // maxsample.rightmin=0;
    // maxsample.leftmax=0;
    // maxsample.rightmax=0;
    // for (current_sample = 0; current_sample < tracksize_samples; current_sample++)
    // {
    //   sample_t s = readsamplesource();
    //   if (s.left < 0){
    //     if (s.left < maxsample.leftmin) maxsample.leftmin = s.left;
    //   }
    //   else {
    //     if (s.left > maxsample.leftmax) maxsample.leftmax = s.left;
    //   }
    //   if (s.right < 0){
    //     if (s.right < maxsample.rightmin) maxsample.rightmin = s.right;
    //   }
    //   else {
    //     if (s.right > maxsample.rightmax) maxsample.rightmax = s.right;
    //   }
    // }
    // //16Bit kann max -32767 bzw + 32767 sein. Wir heben die Amplitude bis auf +/-32000 an
    // signed offset,l,r;
    // if (abs(maxsample.leftmin) > abs(maxsample.leftmax))
    //   l = abs(maxsample.leftmin);  
    // else
    //   l = abs(maxsample.leftmax);
    // if (abs(maxsample.rightmin) > abs(maxsample.rightmax))
    //   r = abs(maxsample.rightmin);  
    // else
    //   r = abs(maxsample.rightmax);

    // if (l > r) offset=200;//32000-l;
    // else offset=200;//32000-r;
    

    // seeksamplesource (tracktimes[i].begin);
    // closewavdest ();		//close destination file
    // openwavdest (outfile, tracksize_samples * 4); //open it again for correct sample start

    // for (current_sample = 0; current_sample < tracksize_samples; current_sample++)
    // //for (current_sample = 0; current_sample < 10; current_sample++)
    // {
    //   sample_t s = readsamplesource();
    //   //printf("left=%d=0x%x  right=%d=0x%x\n",s.left,s.right,s.left,s.right);
    //   if (s.left < 0) 
    //     s.left -= offset;
    //   else 
    //     s.left += offset;
    //   if (s.right < 0) 
    //     s.right -= offset;
    //   else 
    //     s.right += offset;
    //   //printf("ampl=%d  ampl=%d\n",s.left,s.right);
    //   writesampledest(s);
    // }
      

    closewavdest ();		/* close destination */
    delete_filters ();	/* delete filters */
  }
  closewavsource ();		/* close source */
}


/*
[buero@manjaro gramofile]$ cat all.wav.tracks 
# GramoFile Tracks File
#
# This file contains information on track starts/ends. It is automatically
# generated and will be overwritten completely by subsequent track-
# splitting actions on the same audio file.

# Blank lines and lines starting with `#' are ignored.

[Tracks]
# These values are not used (yet), but are included for reference /
# regeneration purposes.
Blocklen=4410
Global_silence_factor=150
Local_silence_factor=5
Min_silence_blocks=20
Min_track_blocks=50
Extra_blocks_start=3
Extra_blocks_end=6

# Below are start/end times of tracks. These are used to create separate
# soundfiles during signal processing. You may modify the computed values
# if you disagree... The block-numbers are those used in the .med file.

Number_of_tracks=2

# Track 1 - blocks 0 to 285 - length: 0:00:28.600
Track01start=0:00:00.000
Track01end=0:00:28.600

# Track 2 - blocks 344 to 502 - length: 0:00:15.900
Track02start=0:00:34.400
Track02end=0:00:50.300

--------------------------------------------------------

*/
#ifndef _FMTHEADERS_H
#define _FMTHEADERS_H	1

#include <sys/types.h>
#include <inttypes.h>

/* Definitions for .VOC files */

#define VOC_MAGIC	"Creative Voice File\032"

#define DATALEN(bp)	((uint32_t)(bp.BlockLen[0]) | \
                         ((uint32_t)(bp.BlockLen[1]) << 8) | \
                         ((uint32_t)(bp.BlockLen[2]) << 16) )

typedef struct vochead
  {
    uint8_t Magic[20];		/* must be VOC_MAGIC */
    uint16_t BlockOffset;	/* Offset to first block from top of file */
    uint16_t Version;		/* VOC-file version */
    uint16_t IDCode;		/* complement of version + 0x1234 */
  }
vochead;

typedef struct blockTC
  {
    uint8_t BlockID;
    uint8_t BlockLen[3];		/* low, mid, high byte of length of rest of block */
  }
blockTC;

typedef struct blockT1
  {
    uint8_t TimeConstant;
    uint8_t PackMethod;
  }
blockT1;

typedef struct blockT8
  {
    uint16_t TimeConstant;
    uint8_t PackMethod;
    uint8_t VoiceMode;
  }
blockT8;

typedef struct blockT9
  {
    u_int SamplesPerSec;
    uint8_t BitsPerSample;
    uint8_t Channels;
    uint16_t Format;
    uint8_t reserved[4];
  }
blockT9;



/* Definitions for Microsoft WAVE format */

/* it's in chunks like .voc and AMIGA iff, but my source say there
   are in only in this combination, so I combined them in one header;
   it works on all WAVE-file I have
 */
typedef struct wavhead
  {
    uint32_t main_chunk;		/* 'RIFF' */
    uint32_t length;		/* Length of rest of file */
    uint32_t chunk_type;		/* 'WAVE' */

    uint32_t sub_chunk;		/* 'fmt ' */
    uint32_t sc_len;		/* length of sub_chunk, =16 (rest of chunk) */
    uint16_t format;		/* should be 1 for PCM-code */
    //uint32_t audioFormat; /*'Microsoft Pulse Code Modulation (PCM) / Uncompressed',*/

    uint16_t modus;		/* 1 Mono, 2 Stereo */
    uint32_t sample_fq;		/* frequence of sample */
    uint32_t byte_p_sec;
    uint16_t byte_p_spl;		/* samplesize; 1 or 2 bytes */
    uint16_t bit_p_spl;		/* 8, 12 or 16 bit */

    uint32_t data_chunk;		/* 'data' */
    uint32_t data_length;		/* samplecount (lenth of rest of block?) */
  }
wavhead;

#endif

/*  Stolen from examples on the web          */
/*  Joshua Weage  gte855f@prism.gatech.edu   */
/*  Switches endianness                      */

#include <sys/types.h>
#include <inttypes.h>
#include "signpr_general.h"
#include "endian.h"

uint16_t
SwapTwoBytes (uint16_t w)
{
  register uint16_t tmp;
  tmp = (w & 0x00FF);
  tmp = ((w & 0xFF00) >> 0x08) | (tmp << 0x08);
  return (tmp);
}

short
SwapTwo (short w)
{
  register short tmp;
  tmp = (w & 0x00FF);
  tmp = ((w & 0xFF00) >> 0x08) | (tmp << 0x08);
  return (tmp);
}

uint32_t
SwapFourBytes (uint32_t dw)
{
  register uint32_t tmp;
  tmp = (dw & 0x000000FF);
  tmp = ((dw & 0x0000FF00) >> 0x08) | (tmp << 0x08);
  tmp = ((dw & 0x00FF0000) >> 0x10) | (tmp << 0x08);
  tmp = ((dw & 0xFF000000) >> 0x18) | (tmp << 0x08);
  return (tmp);
}

sample_t
SwapSample (sample_t sample)
{
  sample.left = SwapTwo (sample.left);
  sample.right = SwapTwo (sample.right);
  return (sample);
}

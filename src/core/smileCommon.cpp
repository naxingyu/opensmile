/*F***************************************************************************
 * 
 * openSMILE - the Munich open source Multimedia Interpretation by 
 * Large-scale Extraction toolkit
 * 
 * This file is part of openSMILE.
 * 
 * openSMILE is copyright (c) by audEERING GmbH. All rights reserved.
 * 
 * See file "COPYING" for details on usage rights and licensing terms.
 * By using, copying, editing, compiling, modifying, reading, etc. this
 * file, you agree to the licensing terms in the file COPYING.
 * If you do not agree to the licensing terms,
 * you must immediately destroy all copies of this file.
 * 
 * THIS SOFTWARE COMES "AS IS", WITH NO WARRANTIES. THIS MEANS NO EXPRESS,
 * IMPLIED OR STATUTORY WARRANTY, INCLUDING WITHOUT LIMITATION, WARRANTIES OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ANY WARRANTY AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE SOFTWARE OR ANY WARRANTY OF TITLE
 * OR NON-INFRINGEMENT. THERE IS NO WARRANTY THAT THIS SOFTWARE WILL FULFILL
 * ANY OF YOUR PARTICULAR PURPOSES OR NEEDS. ALSO, YOU MUST PASS THIS
 * DISCLAIMER ON WHENEVER YOU DISTRIBUTE THE SOFTWARE OR DERIVATIVE WORKS.
 * NEITHER TUM NOR ANY CONTRIBUTOR TO THE SOFTWARE WILL BE LIABLE FOR ANY
 * DAMAGES RELATED TO THE SOFTWARE OR THIS LICENSE AGREEMENT, INCLUDING
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL OR INCIDENTAL DAMAGES, TO THE
 * MAXIMUM EXTENT THE LAW PERMITS, NO MATTER WHAT LEGAL THEORY IT IS BASED ON.
 * ALSO, YOU MUST PASS THIS LIMITATION OF LIABILITY ON WHENEVER YOU DISTRIBUTE
 * THE SOFTWARE OR DERIVATIVE WORKS.
 * 
 * Main authors: Florian Eyben, Felix Weninger, 
 * 	      Martin Woellmer, Bjoern Schuller
 * 
 * Copyright (c) 2008-2013, 
 *   Institute for Human-Machine Communication,
 *   Technische Universitaet Muenchen, Germany
 * 
 * Copyright (c) 2013-2015, 
 *   audEERING UG (haftungsbeschraenkt),
 *   Gilching, Germany
 * 
 * Copyright (c) 2016,	 
 *   audEERING GmbH,
 *   Gilching Germany
 ***************************************************************************E*/


#include <core/smileCommon.hpp>




#ifdef __DATE__
#undef APPDATE
#define APPDATE __DATE__
#endif 

void smilePrintHeader()
{
  SMILE_PRINT(" ");
  SMILE_PRINT(" =============================================================== ");
  SMILE_PRINT("   %s version %s (Rev. %s)",APPNAME, APPVERSION, OPENSMILE_SOURCE_REVISION);
  SMILE_PRINT("   Build date: %s (%s)", APPDATE, OPENSMILE_BUILD_DATE);
  SMILE_PRINT("   Build branch: '%s'", OPENSMILE_BUILD_BRANCH);
  SMILE_PRINT("   (c) %s by %s", APPCPYEAR, APPCPAUTHOR);
  SMILE_PRINT("   All rights reserved. See the file COPYING for license terms.");
  SMILE_PRINT("   %s", APPCPINST);
  SMILE_PRINT(" =============================================================== ");
  SMILE_PRINT(" ");
}

/* check if all elements in the vector are finite. If not, set the element to 0 */
int checkVectorFinite(FLOAT_DMEM *x, long N) 
{
  int i; int f = 1;
  int ret = 1;
  for (i=0; i<N; i++) {
    if (isnan(x[i])) {
      //printf("ERRR!\n X:%f istart: %f iend %f gend %f gstart %f \n iVI %i II %i end %f start %f step %f startV %f\n",X,istart,iend,gend,gstart,ivI,curRange[i].isInit,curRange[i].end,curRange[i].start,curRange[i].step,startV);
      //SMILE_ERR(2,"#NAN# value encountered in output (This value is now automatically set to 0, but somewhere there is a bug)! (index %i)",i);
      x[i] = 0.0; f=0;
      ret = 0;
    } 
    if (isinf(x[i])) {
      //printf("ERRR!\n X:%f istart: %f iend %f gend %f gstart %f \n iVI %i II %i end %f start %f step %f startV %f\n",X,istart,iend,gend,gstart,ivI,curRange[i].isInit,curRange[i].end,curRange[i].start,curRange[i].step,startV);
      //SMILE_ERR(2,"#INF# value encountered in output (This value is now automatically set to 0, but somewhere there is a bug)! (index %i)",i);
      x[i] = 0.0; f=0;
      ret = 0;
    }
  }
  return ret;
}

// realloc and initialize additional memory with zero, like calloc
void * crealloc(void *a, size_t size, size_t old_size)
{
  a = realloc(a,size);
  if ((old_size < size)&&(a!=NULL)) {
    char *b = (char *)a + old_size;
    //fill with zeros:
    bzero((void*)b, size-old_size);
  }
  return a;
}

/* allocate a string and expand vararg format string expression into it */
// WARNING: memory allocated by myvprint must freed by the code calling myvprint!!
char *myvprint(const char *fmt, ...) {
  char *s= (char *)malloc(sizeof(char)*(MIN_LOG_STRLEN+4));
  int LL = MIN_LOG_STRLEN+1;
  //TODO: use vasprintf(); -> only available on linux :(
  if (s==NULL) return NULL;
  va_list ap;
  va_start(ap, fmt);
  int len = vsnprintf(s,MIN_LOG_STRLEN+1, fmt, ap);
  if (len > MIN_LOG_STRLEN) {
    free(s);
    s = (char *)malloc(sizeof(char)*(len+4));
    va_start(ap, fmt);
    len = vsnprintf(s,len+1, fmt, ap);
    LL = len+1;
  } else
  if (len == -1) { // we are probably on windows...
    #ifdef _MSC_VER
    free(s);
    len = _vscprintf(fmt, ap);
    s = (char *)malloc(sizeof(char)*(len+4));
    va_start(ap, fmt);
    len = vsnprintf(s,len+1, fmt, ap);
    LL = len+1;
    #else
    s[0] = 0; // return a null string
    #endif
  }
  va_end(ap);
  // ensure the last two characters are 0 !:
  s[LL] = 0;
  s[LL+1] = 0;
  return s;
}

void * memdup(const void *in, size_t size)
{
  void *ret = malloc(size);
  if (ret!=NULL) memcpy(ret,in,size);
  return ret;
}



#ifdef __WINDOWS
void gettimeofday(timeval* p, void* tz /* IGNORED */){
  union {
    long long ns100; /*time since 1 Jan 1601 in 100ns units */
    FILETIME ft;
  } _now;

  GetSystemTimeAsFileTime( &(_now.ft) );
  p->tv_usec=(long)((_now.ns100 / 10LL) % 1000000LL );
  p->tv_sec= (long)((_now.ns100-(116444736000000000LL))/10000000LL);
  return;
}
#endif /* __WINDOWS */


#ifdef __MACOS
int clock_gettime (int clock_id, struct timespec *tp)
{
	struct timeval tv;
	int retval = gettimeofday (&tv, NULL);
	if (retval == 0)
		TIMEVAL_TO_TIMESPEC (&tv, tp); // Convert into `timespec'.
	return retval;
}
#endif

#if defined(__HAVENT_GNULIBS) || defined(__ANDROID__)

#define MIN_CHUNK 64
long getstr (char **lineptr, size_t *n, FILE *stream, char terminator, int offset)
{
  int nchars_avail;		/* Allocated but unused chars in *LINEPTR.  */
  char *read_pos;		/* Where we're reading into *LINEPTR. */
  long ret;

  if (!lineptr || !n || !stream)
    {
      return -1;
    }

  if (!*lineptr)
    {
      *n = MIN_CHUNK;
      *lineptr = (char *)malloc (*n);
      if (!*lineptr)
	{
	  return -1;
	}
    }

  nchars_avail = (int)(*n) - offset;
  read_pos = *lineptr + offset;

  for (;;)
    {
      register int c = getc (stream);


      /* We always want at least one char left in the buffer, since we
	 always (unless we get an error while reading the first char)
	 NUL-terminate the line buffer.  */


      if (nchars_avail < 2)
	{
	  if (*n > MIN_CHUNK)
	    *n *= 2;
	  else
	    *n += MIN_CHUNK;

	  nchars_avail = (int)( (long long)(*n) + (long long)*lineptr - (long long)read_pos );
	  *lineptr = (char *)realloc (*lineptr, *n);
	  if (!*lineptr)
	    {
	      return -1;
	    }
	  read_pos = *n - nchars_avail + *lineptr;
	}

      if (ferror (stream))
	{
	  return -1;
	}

      if (c == EOF)
	{
	  /* Return partial line, if any.  */
	  if (read_pos == *lineptr)
	    return -1;
	  else
	    break;
	}

      *read_pos++ = c;
      nchars_avail--;

      if (c == terminator)
	/* Return the line.  */
	break;
   }

  /* Done - NUL terminate and return the number of chars read.  */
  *read_pos = '\0';

  ret = (long)( (long long)read_pos - ((long long)*lineptr + (long long)offset) );
  return ret;
}

long smile_getline (char **lineptr, size_t *n, FILE *stream)
{
  return getstr (lineptr, n, stream, '\n', 0);
}
#endif


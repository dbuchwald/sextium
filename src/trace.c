#include <stdio.h>
#include <stdarg.h> /* va_list */
#include <stdlib.h> /* getenv */
#include <string.h> /* strcmp */
#include <time.h> /* strftime */
#include "trace.h"

#define __MAX_TRACE_TIMESTAMP_LENGTH 512 
#define __TRACE_TRACE_LEVEL_ENV "TRACE_LEVEL"
#define __TRACE_TRACE_FILENAME "TRACE_FILE"
#define __TRACE_DEFAULTLEVEL TRACE_ERROR

static const char __TRACE_LEVEL_OFF[] = "OFF";
static const char __TRACE_LEVEL_ERROR[] = "ERROR";
static const char __TRACE_LEVEL_WARNING[] = "WARNING";
static const char __TRACE_LEVEL_INFO[] = "INFO"; 
static const char __TRACE_LEVEL_DEBUG[] = "DEBUG";

#define __TRACE_NO_LEVEL_DEFINED 1
#define __TRACE_LEVEL_DEFINED 2
#define __TRACE_WRONG_LEVEL_DEFINED 3

#define __TRACE_NO_FILE_DEFINED 1
#define __TRACE_FILE_DEFINED 2
#define __TRACE_FINE_NOT_OPEN 3

static FILE* _trace_file;
static int _trace_initialized;
static TRACE_ERRORLEVEL _trace_level;

static const char* GetLevelString(const TRACE_ERRORLEVEL level)
{
  switch (level)
  {
    case TRACE_OFF:
      return __TRACE_LEVEL_OFF;
    case TRACE_ERROR:
      return __TRACE_LEVEL_ERROR;
    case TRACE_WARNING:
      return __TRACE_LEVEL_WARNING;
    case TRACE_INFO:
      return __TRACE_LEVEL_INFO;
    case TRACE_DEBUG:
      return __TRACE_LEVEL_DEBUG;
  }

  return NULL;
}

static void InitializeTrace()
{
  int trace_level_defined;
  int trace_file_defined;

  if (!getenv(__TRACE_TRACE_LEVEL_ENV))
  {
    _trace_level = __TRACE_DEFAULTLEVEL;
    trace_level_defined=__TRACE_NO_LEVEL_DEFINED;
  }
  else if (!strcmp(getenv(__TRACE_TRACE_LEVEL_ENV), __TRACE_LEVEL_OFF))
  {
    _trace_level = TRACE_OFF;
    trace_level_defined=__TRACE_LEVEL_DEFINED;
  }
  else if (!strcmp(getenv(__TRACE_TRACE_LEVEL_ENV), __TRACE_LEVEL_ERROR))
  {
    _trace_level = TRACE_ERROR;
    trace_level_defined=__TRACE_LEVEL_DEFINED;
  }
  else if (!strcmp(getenv(__TRACE_TRACE_LEVEL_ENV), __TRACE_LEVEL_WARNING))
  {
    _trace_level = TRACE_WARNING;
    trace_level_defined=__TRACE_LEVEL_DEFINED;
  }
  else if (!strcmp(getenv(__TRACE_TRACE_LEVEL_ENV), __TRACE_LEVEL_INFO))
  {
    _trace_level = TRACE_INFO;
    trace_level_defined=__TRACE_LEVEL_DEFINED;
  }
  else if (!strcmp(getenv(__TRACE_TRACE_LEVEL_ENV), __TRACE_LEVEL_DEBUG))
  {
    _trace_level = TRACE_DEBUG;
    trace_level_defined=__TRACE_LEVEL_DEFINED;
  }
  else
  {
    _trace_level = __TRACE_DEFAULTLEVEL;
    trace_level_defined=__TRACE_WRONG_LEVEL_DEFINED;
  }

  if (!getenv(__TRACE_TRACE_FILENAME))
  {
    _trace_file = stdout;
    trace_file_defined=__TRACE_NO_FILE_DEFINED;
  }
  else if (!(_trace_file=fopen(getenv(__TRACE_TRACE_FILENAME), "a")))
  {
    _trace_file = stdout;
    trace_file_defined=__TRACE_FINE_NOT_OPEN;
  }
  else
  {
    trace_file_defined=__TRACE_FILE_DEFINED;
  }
  _trace_initialized=1;

  switch(trace_level_defined)
  {
    case __TRACE_NO_LEVEL_DEFINED:
      TRACE(TRACE_ERROR, "No trace level selected, defaulting to %s", GetLevelString(__TRACE_DEFAULTLEVEL));
      break;
    case __TRACE_LEVEL_DEFINED:
      TRACE(TRACE_INFO, "Trace level: %s", GetLevelString(_trace_level));
      break;
    case __TRACE_WRONG_LEVEL_DEFINED:
      TRACE(TRACE_ERROR, "Wrong trace level selected: %s, defaulting to %s", getenv(__TRACE_TRACE_LEVEL_ENV), GetLevelString(__TRACE_DEFAULTLEVEL));
      break;
  }

  switch(trace_level_defined)
  {
    case __TRACE_NO_FILE_DEFINED:
      TRACE(TRACE_WARNING, "No trace file specified, outputting to standard output");
      break;
    case __TRACE_FINE_NOT_OPEN:
      TRACE(TRACE_ERROR, "Unable to open trace file, outputting to standard output");
      break;
  }
}

void PrintTraceLine(const char* function, const char* source_file, const int source_line, const TRACE_ERRORLEVEL level, const char* format, ...)
{
  va_list vl;
  va_start(vl, format);
  static char timestamp[__MAX_TRACE_TIMESTAMP_LENGTH];  

  if (!_trace_initialized)
  {
    InitializeTrace();
  } 

  if (level <= _trace_level)
  {
    time_t t;    
    struct tm *tmp;
    time(&t);
    tmp = localtime(&t);
    strftime(timestamp, sizeof(timestamp), "%Y.%m.%d %H:%M:%S", tmp);
    fprintf(_trace_file, "[%s] %s ", timestamp, GetLevelString(level));
    fprintf(_trace_file, "<%s> (%s:%d) > ", function, source_file, source_line);
    vfprintf(_trace_file, format, vl);
    fprintf(_trace_file, "\n");
    fflush(_trace_file);
  }
}

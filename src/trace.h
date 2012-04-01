
typedef enum {
  TRACE_OFF=0,
  TRACE_ERROR=1,
  TRACE_WARNING=2,
  TRACE_INFO=3,
  TRACE_DEBUG=4
} TRACE_ERRORLEVEL;

void PrintTraceLine(const char* function, const char* source_file, const int source_line, const TRACE_ERRORLEVEL level, const char* format, ...);

#define TRACE(LEVEL, ...) PrintTraceLine(__func__, __FILE__, __LINE__, LEVEL, __VA_ARGS__)


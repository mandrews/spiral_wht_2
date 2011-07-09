#include "config.h"
#ifndef HAVE_GETLINE
#include <stdio.h>
#ifdef HAVE_FGETLN
inline
int /* ssize_t in GNU specification */
getline(char **lineptr, size_t *n, FILE *stream) 
{
  *lineptr = fgetln(stream, n);
  return (lineptr == NULL) ? -1 : 0;
}
#else
#error "Must have either getline or fgetln"
#endif /* HAVE_FGETLN */
#endif /* HAVE_GETLINE */

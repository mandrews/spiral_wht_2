#include "wht.h"
#include "registry.h"

#include "scanner.h"

static char warn_msg[MAX_MSG_LEN];
static char erro_msg[MAX_MSG_LEN];

/* This symbol points to the root of the parsed wht tree */
extern Wht *wht_root;

Wht *
parse(char *in)
{
  Wht *W;
  YY_BUFFER_STATE buf;

  buf = yy_scan_string(in);

  yyparse();

  W = wht_root;

  W->transform(W);

  yy_delete_buffer(buf);

  return W;
}

bool 
accepted(Wht *W)
{
  return (strlen(erro_msg_get()) == 0);
}

void
info(void)
{
  printf("CC: %s\n", WHT_CC);
  printf("CFLAGS: %s\n", WHT_CFLAGS);
  printf("type: %s\n", WHT_TYPE_STRING);
  printf("max_unroll: %d\n", WHT_MAX_UNROLL);
  printf("max_interleave: %d\n", WHT_MAX_INTERLEAVE);
  printf("vector_size: %d\n", WHT_VECTOR_SIZE);
}

codelet_transform_fp 
codelet_transform_lookup(const char *name, size_t params)
{
  codelet_transform_entry *p;

  for (p = (codelet_transform_entry *) codelet_transforms_registry; p != NULL && (codelet_transform_fp) p->call != NULL; ++p)
    if ((strncmp(name, p->name, MAX_CODELET_NAME_SIZE) == 0) && (params == p->params))
      return p->call;

  return NULL;
}

void
codelet_transform(Wht *W, const char *name, int params[], size_t n)
{
  codelet_transform_fp f;
  int i;

  f = codelet_transform_lookup(name, n);

  if (f == NULL)
    wht_exit("%s was not registered in the transform table", name);

  W->transform = f;

  for (i = 0; i < n; i++)
    W->params[i] = params[i];

  i_free(W->name);

  if (n > 0)
    W->name = append_params_to_name(name, params, n);
  else
    W->name = strdup(name);
}


char * 
warn_msg_get() 
{ 
  return warn_msg;
}

void 
warn_msg_set(char *format, ...) 
{ 
  va_list ap; 
  va_start(ap, format); 
  vsnprintf(warn_msg, MAX_MSG_LEN, format, ap); 
  va_end(ap); 
}

char * 
erro_msg_get() 
{ 
  return erro_msg;
}

void 
erro_msg_set(char *format, ...) 
{ 
  va_list ap; 
  va_start(ap, format); 
  vsnprintf(erro_msg, MAX_MSG_LEN, format, ap); 
  va_end(ap); 
}

char *
append_params_to_name(const char *name, int params[], size_t n)
{
  const size_t DIGIT_SIZE = 32;
  size_t bufsize;
  int i;
  char *buf;
  char tmp[DIGIT_SIZE];

  bufsize = strlen(name) + DIGIT_SIZE*n + (n-1) + 3; 
   /* n DIGITS + (n-1) COMMAS + '(' + ')' + '\0' */

  buf = i_malloc(sizeof(char) * bufsize);

  buf[0] = '\0';
  strncat(buf, name, strlen(name));
  strncat(buf, "(", 1);

  for (i = 0; i < n; i++) {
    snprintf(tmp, DIGIT_SIZE, "%d", params[i]);
    strncat(buf, tmp, strlen(tmp));

    if (i != n - 1)
      strncat(buf, ",", 1);
  }

  strncat(buf, ")", 1);

  return buf;
}


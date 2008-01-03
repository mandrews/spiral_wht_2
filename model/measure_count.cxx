extern "C" {
#include "wht.h"
#include "measure.h"

#include "getopt.h"
}

#include "labeled_vector.h"


void 
count(labeled_vector *sample, char *extn, char *metric)
{
  labeled_vector::iterator i;
  struct stat *stat;
  char *tmp;
  Wht *W;

  i = sample->begin();

  for (; i != sample->end(); i++) {
    tmp = (char *) i->first.c_str();

    W = wht_parse(tmp);

    stat = measure(W, extn, metric, true, 1, 1);

    i->second = (double) stat->mean;

    wht_free(W);
    free(stat);
  }

  save_labeled_vector(stdout, sample); 

}


static void
usage() 
{
  printf("Usage: wht_measure_count -b BASIS -s SAMPLE [OPTIONS]\n\n");
  printf("    -h        Show this help message.\n");
  printf("    -v        Show build information.\n");
  printf("    -s        Sampling WHT plans.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *sample_file;
  char *extn, *metric;
  int c;

  sample_file  = NULL;
  extn = NULL;
  metric = NULL;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvs:e:m:")) != -1)
    switch (c) {
      case 's':
        sample_file = optarg;
        break;
      case 'e':
        extn = optarg;
        break;
      case 'm':
        metric = optarg;
        break;
      case 'h':
        usage();
      case 'v':
        wht_info();
        exit(0);
      default:
        usage();
    }

  if (sample_file == NULL)
    usage();

  FILE *fd;
  labeled_vector *sample;

  fd = fopen(sample_file, "r");
  sample = load_labeled_vector(fd);
  fclose(fd);

  count(sample, extn, metric);

  delete sample;

  return 0;
}

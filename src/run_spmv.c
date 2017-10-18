#include <errno.h>
#include <getopt.h>
#include <gsl/gsl_spmatrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static int verbose = 0;
static int help = 0;

int test (size_t m,
	  size_t n,
	  size_t r,
	  size_t c,
	  size_t nnz,
	  const size_t *ptr,
	  const size_t *ind,
	  const double *data,
          int trials,
          int verbose);

static void usage () {
  fprintf(stderr,"usage: spmv [options] <input>\n"
  "  <input>                    MatrixMarket file (multiply this matrix)\n"
  "  -t, --trials <arg>         Number of trials to run\n"
  "  -r, --block_r <arg>        Row block size\n"
  "  -c, --block_c <arg>        Column block size\n"
  "  -v, --verbose              Verbose mode\n"
  "  -q, --quiet                Quiet mode\n"
  "  -h, --help                 Display help message\n");
}

int main (int argc, char **argv) {

  size_t r = 1;
  size_t c = 1;
  int trials = 1;

  /* Beware. Option parsing below. */
  long longarg;
  double doublearg;
  while (1) {
    static char *options = "B:e:d:t:cCrRvqh";
    static struct option long_options[] = {
        {"block_r",  required_argument, 0, 'r'},
        {"block_c",  required_argument, 0, 'c'},
        {"trials",   required_argument, 0, 't'},
        {"verbose",   no_argument, &verbose, 1},
        {"quiet",     no_argument, &verbose, 0},
        {"help",      no_argument, &help,    1},
        {0, 0, 0, 0}
      };

    /* getopt_long stores the option index here. */
    int option_index = 0;

    int c = getopt_long (argc, argv, options,
                     long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    if (c == 0 && long_options[option_index].flag == 0)
      c = long_options[option_index].val;

    switch (c) {
      case 0:
        /* If this option set a flag, do nothing else now. */
        break;

      case 'r':
        errno = 0;
        longarg = strtol(optarg, 0, 10);
        if (errno != 0 || longarg < 1) {
          printf("option -r takes an integer maximum block size >= 1\n");
          usage();
          return 1;
        }
        r = longarg;
        break;

      case 'c':
        errno = 0;
        longarg = strtol(optarg, 0, 10);
        if (errno != 0 || longarg < 1) {
          printf("option -c takes an integer maximum block size >= 1\n");
          usage();
          return 1;
        }
        r = longarg;
        break;

      case 't':
        errno = 0;
        longarg = strtol(optarg, 0, 10);
        if (errno != 0 || longarg < 1) {
          printf("option -t takes an integer number of trials >= 1\n");
          usage();
          return 1;
        }
        trials = longarg;
        break;

      case 'v':
        verbose = 1;
        break;

      case 'q':
        verbose = 0;
        break;

      case 'h':
        help = 1;
        break;

      case '?':
        usage();
        return 1;

      default:
        abort();
    }
  }

  if (help) {
    printf("Run a fill estimation algorithm!\n");
    usage();
    return 0;
  }

  if (argc - optind > 1) {
    printf("<input> cannot be more than one file\n");
    usage();
    return 1;
  }

  if (argc - optind < 1) {
    printf("<input> not specified\n");
    usage();
    return 1;
  }

  struct stat statthing;
  if (stat(argv[optind], &statthing) < 0 || !S_ISREG(statthing.st_mode)){
    printf("<input> must be filename of MatrixMarket matrix\n");
    usage();
    return 1;
  }

  FILE *f = fopen(argv[optind], "r");
  gsl_spmatrix *triples = gsl_spmatrix_fscanf(f);
  fclose(f);
  if (triples == 0) {
    printf("<input> must be filename of MatrixMarket matrix\n");
    usage();
    return 1;
  }

  int ret = test(triples->size1, triples->size2, r, c, triples->nz, triples->i, triples->p, triples->data, trials, verbose);

  gsl_spmatrix_free(triples);

  return ret;

}

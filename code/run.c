#include <errno.h>
#include <getopt.h>
#include <gsl/gsl_spmatrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static int clock = 1;
static int results = 0;
static int verbose = 0;
static int help = 0;

int test (size_t m,
          size_t n,
          size_t nnz,
          const size_t *ptr,
          const size_t *ind,
          size_t B,
          int trials,
          int clock,
          int results,
          int verbose);

char *name ();

static void usage () {
  fprintf(stderr,"usage: %s [options] <input>\n"
  "  <input>                    MatrixMarket file (estimate fill of this matrix)\n"
  "  -B, --max-block-size <arg> Maximum block dimension for fill estimates\n"
  "  -t, --trials <arg>         Number of trials to run\n"
  "  -c, --clock                Display timing information\n"
  "  -d, --noclock              Do not display timing information\n"
  "  -r, --results              Display fill estimates for all trials\n"
  "  -s, --noresults            Do not display fill estimates\n"
  "  -v, --verbose              Verbose mode\n"
  "  -q, --quiet                Quiet mode\n"
  "  -h, --help                 Display help message\n", name());
}

int main (int argc, char **argv) {

  size_t B = 5;
  int trials = 1;

  /* Beware. Option parsing below. */
  long longarg;
  while (1) {
    static char *options = "B:t:cdrsvqh";
    static struct option long_options[] = {
        {"max-block-size", required_argument, 0, 'B'},
        {"trials",         required_argument, 0, 't'},
        {"clock",     no_argument, &clock,   1},
        {"noclock",   no_argument, &clock,   0},
        {"results",   no_argument, &results, 1},
        {"noresults", no_argument, &results, 0},
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

      case 'B':
        errno = 0;
        longarg = strtol(optarg, 0, 10);
        if (errno != 0 || longarg < 1) {
          printf("option -B takes an integer maximum block size >= 1\n");
          usage();
          return 1;
        }
        B = longarg;
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

      case 'c':
        results = 1;
        break;

      case 'd':
        results = 0;
        break;

      case 'r':
        results = 1;
        break;

      case 's':
        results = 0;
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
  if (errno != 0 || longarg < 1) {
    printf("<input> must be filename of MatrixMarket matrix\n");
    usage();
    return 1;
  }

  gsl_spmatrix *csr = gsl_spmatrix_crs(triples);
  gsl_spmatrix_free(triples);

  int ret = test(csr->size1, csr->size2, csr->nz, csr->p, csr->i, B, trials, clock, results, verbose);

  gsl_spmatrix_free(csr);

  return ret;
}

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
          size_t nnz,
          const size_t *ptr,
          const size_t *ind,
          size_t B,
          int verbose);

int main (int argc, char **argv) {

  size_t B = 12;

  /* Beware. Option parsing below. */
  long longarg;
  while (1) {
    static char *options = "vqB:m:";
    static struct option long_options[] = {
        {"verbose", no_argument,       &verbose, 1},
        {"quiet",   no_argument,       &verbose, 0},
        {"help",    no_argument,       &help,    1},
        {"max-block-size",  required_argument, 0, 'B'},
        {"matrix",    required_argument, 0, 'm'},
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

      case 'v':
        verbose = 1;
        break;

      case 'q':
        verbose = 0;
        break;

      case 'h':
        help = 1;
        break;

      case 'B':
        errno = 0;
        longarg = strtol(optarg, 0, 10);
        if (errno != 0 || longarg < 1) {
          printf("option -B takes an integer maximum block size >= 1\n");
          abort();
        }
        B = longarg;
        break;

      case '?':
        /* getopt_long already printed an error message. */
        break;

      default:
        abort();
    }
  }

  if (optind != argc - 1) {
    printf("must specify one matrix to test\n");
    abort();
  }

  struct stat statthing;
  if (stat(argv[optind], &statthing) < 0 || !S_ISREG(statthing.st_mode)){
    printf("must specify filename of MatrixMarket matrix\n");
    abort();
  }

  FILE *f = fopen(argv[optind], "r");
  gsl_spmatrix *triples = gsl_spmatrix_fscanf(f);
  fclose(f);
  if (errno != 0 || longarg < 1) {
    printf("must specify filename of MatrixMarket matrix\n");
    abort();
  }

  gsl_spmatrix *csr = gsl_spmatrix_crs(triples);
  gsl_spmatrix_free(triples);

  int ret = test(csr->size1, csr->size2, csr->nz, csr->p, csr->i, B, verbose);

  gsl_spmatrix_free(csr);

  return ret;
}

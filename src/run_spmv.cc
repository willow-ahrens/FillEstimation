#include <errno.h>
#include <getopt.h>
#include <taco.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

extern "C" {

int test (int m,
          int n,
          int r,
          int c,
          int nnz,
          const int *ind,
          const int *ptr,
          const double *data,
          int trials,
          int verbose);

static void usage () {
  fprintf(stderr,"usage: spmv [options] <input>\n"
  "  <input>                    MatrixMarket file (multiply this matrix)\n"
  "  -r, --block_r <arg>        Row block size\n"
  "  -c, --block_c <arg>        Column block size\n"
  "  -t, --trials <arg>         Number of trials to run\n"
  "  -v, --verbose              Verbose mode\n"
  "  -q, --quiet                Quiet mode\n"
  "  -h, --help                 Display help message\n");
}

}

int main (int argc, char **argv) {

  int verbose = 0;
  int help = 0;

  int b_r = 1;
  int b_c = 1;
  int trials = 1;

  /* Beware. Option parsing below. */
  long longarg;
  double doublearg;
  while (1) {
    const char *options = "r:c:t:vqh";
    const struct option long_options[] = {
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
          printf("option -r takes an integer column block size >= 1\n");
          usage();
          return 1;
        }
        b_r = longarg;
        break;

      case 'c':
        errno = 0;
        longarg = strtol(optarg, 0, 10);
        if (errno != 0 || longarg < 1) {
          printf("option -c takes an integer column block size >= 1\n");
          usage();
          return 1;
        }
        b_c = longarg;
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

  auto csr = taco::read(argv[optind], taco::CSR, true);

  int ret = test(csr.getDimension(0), csr.getDimension(1), b_r, b_c, csr.getStorage().getValues().getSize(), (int*)csr.getStorage().getIndex().getModeIndex(1).getIndexArray(0).getData(), (int*)csr.getStorage().getIndex().getModeIndex(1).getIndexArray(1).getData(), (double*)csr.getStorage().getValues().getData(), trials, verbose);

  return ret;

}

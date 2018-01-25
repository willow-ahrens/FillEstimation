#include <errno.h>
#include <getopt.h>
#include <taco.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

extern "C" {
#include "util.h"

int test (int m,
          int n,
          int nnz,
          const int *ptr,
          const int *ind,
          int B,
          double epsilon,
          double delta,
          double sigma,
          int trials,
          int clock,
          int results,
          int verbose);

char *name ();

static void usage () {
  fprintf(stderr,"usage: %s [options] <input>\n"
  "  <input>                    MatrixMarket file (estimate fill of this matrix)\n"
  "  -g, --rng-seed <arg>       Seed for random number generator\n"
  "  -B, --max-block-size <arg> Maximum block dimension for fill estimates\n"
  "  -e, --epsilon <arg>        Be accurate to relative error epsilon\n"
  "  -d, --delta <arg>          With probability (1 - delta)\n"
  "  -s, --sigma <arg>          Examine block rows With probability sigma\n"
  "  -t, --trials <arg>         Number of trials to run\n"
  "  -c, --clock                Display timing information\n"
  "  -C, --noclock              Do not display timing information\n"
  "  -r, --results              Display fill estimates for all trials\n"
  "  -R, --noresults            Do not display fill estimates\n"
  "  -v, --verbose              Verbose mode\n"
  "  -q, --quiet                Quiet mode\n"
  "  -h, --help                 Display help message\n", name());
}

}

int main (int argc, char **argv) {

  int clock = 1;
  int results = 0;
  int verbose = 0;
  int help = 0;

  int B = 12;
  double epsilon = 0.1;
  double delta = 0.01;
  double sigma = 0.02;
  int trials = 1;

  /* Beware. Option parsing below. */
  long longarg;
  double doublearg;
  while (1) {
    const char *options = "g:B:e:s:d:t:cCrRvqh";
    const struct option long_options[] = {
        {"rng-seed", required_argument, 0, 'g'},
        {"max-block-size", required_argument, 0, 'B'},
        {"trials",         required_argument, 0, 't'},
        {"epsilon", required_argument, 0, 'e'},
        {"delta", required_argument, 0, 'd'},
        {"sigma", required_argument, 0, 's'},
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

      case 'g':
        errno = 0;
        longarg = strtol(optarg, 0, 10);
        if (errno != 0 || longarg < 0) {
          printf("option -g takes an integer seed >= 0\n");
          usage();
          return 1;
        }
        random_seed(longarg);
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

      case 'e':
        errno = 0;
        doublearg = strtod(optarg, 0);
        if (errno != 0 || doublearg < 0.0) {
          printf("option -e takes a desired relative error >= 0.0\n");
          usage();
          return 1;
        }
        epsilon = doublearg;
        break;

      case 'd':
        errno = 0;
        doublearg = strtod(optarg, 0);
        if (errno != 0 || doublearg < 0.0 || doublearg > 1.0) {
          printf("option -d takes a desired probability >= 0.0 and <= 1.0\n");
          usage();
          return 1;
        }
        delta = doublearg;
        break;

      case 's':
        errno = 0;
        doublearg = strtod(optarg, 0);
        if (errno != 0 || doublearg < 0.0 || doublearg > 1.0) {
          printf("option -s takes a desired probability >= 0.0 and <= 1.0\n");
          usage();
          return 1;
        }
        sigma = doublearg;
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
        clock = 1;
        break;

      case 'C':
        clock = 0;
        break;

      case 'r':
        results = 1;
        break;

      case 'R':
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

  auto csr = taco::read(argv[optind], taco::CSR, true);

  int ret = test(csr.getDimension(0), csr.getDimension(1), csr.getStorage().getValues().getSize(), (int*)csr.getStorage().getIndex().getModeIndex(1).getIndexArray(0).getData(), (int*)csr.getStorage().getIndex().getModeIndex(1).getIndexArray(1).getData(), B, epsilon, delta, sigma, trials, clock, results, verbose);

  return ret;
}

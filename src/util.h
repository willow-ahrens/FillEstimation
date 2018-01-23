#ifndef max
    #define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
    #define min(a,b) ((a) < (b) ? (a) : (b))
#endif

//lo is inclusive, hi is exclusive

int random_range (int lo, int hi);
double random_uniform ();
void sort (int *stuff, int n);
int search (const int *stuff, int lo, int hi, int key);
int search_strict (const int *stuff, int lo, int hi, int key);

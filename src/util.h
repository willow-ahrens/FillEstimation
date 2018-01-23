//lo is inclusive, hi is exclusive

inline int max(int a, int b) {return a > b ? a : b;};
inline int min(int a, int b) {return a < b ? a : b;};
int random_range (int lo, int hi);
double random_uniform ();
void sort (int *stuff, int n);
int search (const int *stuff, int lo, int hi, int key);
int search_strict (const int *stuff, int lo, int hi, int key);

typedef double v2df __attribute__ ((vector_size(16)));
typedef double v4df __attribute__ ((vector_size(32)));

int calc_1(int x, int y, int limit, double inv_w, double inv_h);
void calc_2(int x, int y, int limit, double inv_w, double inv_h, int* r);
void calc_4(int x, int y, int limit, double inv_w, double inv_h, int* r);

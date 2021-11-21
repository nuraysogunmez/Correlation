#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


int main(int argn, char *arg[]) {
  double **a;
  double v[3][3], w[3];
  int i, nres, rr[100000], resid, residold;
  int m, n;
  FILE *fd, *fd1, *fp, *ff;
  char line[1024], key[128];
  double beta, id[2000];
  
  // open for reading only
    fd = fopen(arg[1], "r");
    fd1  = fopen("a.dat", "r");  // beta factor data

  // open for writing ony
    fp = fopen ("50ns-450ns_first_meanNetTE_bb+1sc.pdb", "w");   // new coordinate file with user defined beta factor values
 nres = 309;

    // read the lines, and advance 'm' only for those that with start 'ATOM'
    // Note: '%*s' means ignore the string; do not convert
    
    m = 0;
    for (i=0;i<nres;i++){
        while (fgets(line, 1024, fd1)) {
            sscanf(line, "%lf", &beta);
            id[m] = beta;
            m++;
        }
    }
    printf("%3d\n", m);
    
  m = 0;
  residold = 33;  // first residue id!
    
  while (fgets(line, 1024, fd)) {
    sscanf(line, "%s %*s %*s %*s %*s %3d %*s %*s %*s %*s %lf %*s", key, &resid, &beta);
      if (strcmp(key,"ATOM")==0){
          line[62]=0;
          fprintf(fp, "%s", line);
          if (resid != residold){
              m++;
              residold = resid;
          }
          fprintf(fp, "%4.2lf\n", id[m]);
      }
  }

  return 0;
}

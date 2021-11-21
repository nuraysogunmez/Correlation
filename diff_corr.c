#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


int main(int argn, char *arg[]) {
  
    FILE *fd1, *fd2, *fp, *fpnew;
    int i, j, k, m, nres, nconf;
    char line[1024], key[128], name[128];
    double corrb[400][400], corra[400][400], diff[400][400];
    double *a, *b;
    
    if (argn!=3) {
        fprintf(stderr, "Usage: %s <filename> <run>\n", arg[0]);
        return 1;
    }
 
// open for reading only
    fd1 = fopen(arg[1], "r");
    fd2 = fopen(arg[2], "r");

    if ((fd1 == NULL) | (fd2 == NULL)) {
        fprintf(stderr, "Cannot open %s %s.\n", arg[1], arg[2]);
        return 1;
    }

    // open for writing only
    fp = fopen ("diff_corr.dat", "w");
    fpnew = fopen ("diff_corr_new.dat", "w");

    nres = 311;
    
// get the total number of lines
// qso we allocate (more than) enough space for x,y,z arrays
    m = 0;
    while (fgets(line, 1024, fd1)) {
        m++;
    }
    printf("%d\n",m);

    m = 0;
    while (fgets(line, 1024, fd2)) {
        m++;
    }
    printf("%d\n",m);
 
// allocate memory for tmp x, y, z coordinate arrays
    a = malloc(sizeof(double)*m);
    b = malloc(sizeof(double)*m);
    
// rewind to the beginning of the file
    rewind(fd1);
    rewind(fd2);
    
// now reading the correlation before process and storing them
    m = 0;
    while (fgets(line, 1024, fd1)) {
        sscanf(line, "%*d %*d %lf\n", &a[m]);
        m++;
    }
    m = 0;
    printf("%d\n",m);
    for (i=0;i<nres;i++) {
        for (j=0;j<nres;j++){
            corrb[i][j] = a[m];
            m++;
        }
    }
    printf("%d\n",m);
   
    // now reading the correlation after process and storing them
    m = 0;
    while (fgets(line, 1024, fd2)) {
        sscanf(line, "%*d %*d %lf\n", &a[m]);
        m++;
    }
    m = 0;
    printf("%d\n",m);
    for (i=0;i<nres;i++) {
        for (j=0;j<nres;j++){
            corra[i][j] = a[m];
            m++;
        }
    }
    printf("%d\n",m);
    
// calculating the difference between two correlations

    for (i=0;i<nres;i++) {
        for (j=0;j<nres;j++)
            diff[i][j] = corra[i][j]-corrb[i][j];
    }
   
// print out the results
    for (i=0;i<nres;i++) {
        for (j=0;j<nres;j++){
            fprintf(fp, "%5d %5d %8.4lf %8.4lf %8.4lf\n", i+32, j+32, diff[i][j], corrb[i][j], corra[i][j]);
            if ((corrb[i][j] > 0.0) && (corra[i][j] < 0.0) && (diff[i][j] < 0))
                fprintf(fpnew, "%5d %5d %8.4lf %8.4lf %8.4lf\n", i+32, j+32, diff[i][j], corrb[i][j], corra[i][j]);
        }
    }
    
  return 0;
}


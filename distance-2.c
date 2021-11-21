#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(int argn, char *arg[]) {
  
    FILE *fd1, *fd2;
    FILE *fp;
    int j, k, m;
    char line[1024];
    double *xa, *ya, *za, *xb, *yb, *zb;
    double dista[400][400], distb[400][400];
    double diff[400][400];
 
    if (argn!=3) {
        fprintf(stderr, "Usage: %s <filename> <run1> <run2>\n", arg[0]);
        return 1;
    }
 
// open for reading only
    fd1 = fopen(arg[1], "r");
    fd2 = fopen(arg[2], "r");

    if (fd1 == NULL) {
        fprintf(stderr, "Cannot open %s.\n", arg[1]);
        return 1;
    }
    if (fd2 == NULL) {
        fprintf(stderr, "Cannot open %s.\n", arg[2]);
        return 1;
    }
// open for writing only
    fp = fopen ("distance_difference.dat", "w");

// get the total number of lines
// so we allocete (more than) enough space for x,y,z arrays
    m = 0; while (fgets(line, 1024, fd1)) m++;

// allocate memory for tmp x, y, z coordinate arrays
    xb = malloc(sizeof(double)*m);
    yb = malloc(sizeof(double)*m);
    zb = malloc(sizeof(double)*m);
    xa = malloc(sizeof(double)*m);
    ya = malloc(sizeof(double)*m);
    za = malloc(sizeof(double)*m);

// rewind to the beginning of the file
    rewind(fd1);
    rewind(fd2);
    
// now reading the average coordinates
    m = 0;
    while (fgets(line, 1024, fd1)) {
        sscanf(line, "%*d %lf %lf %lf", &(xb[m]), &(yb[m]), &(zb[m]));
        printf("test\n");
        printf("%d %lf %lf %lf\n", m,xb[m],yb[m],zb[m]);
        m++;
    }
    m = 0;
    while (fgets(line, 1024, fd2)) {
        sscanf(line, "%*d %lf %lf %lf", &(xa[m]), &(ya[m]), &(za[m]));
        m++;
    }
    //printf("test\n");
    
// calculating the distances between residue i and j (before)
    
    for (j=0;j<m;j++){
        for (k=0;k<m;k++){
            printf("%3d %3d %5.2lf %5.2lf %5.2lf %5.2lf %5.2lf %5.2lf\n", j,k, xb[j],yb[j],zb[j],xb[k],yb[k],zb[k]);
            distb[j][k] = sqrt((xb[j] - xb[k])*(xb[j] - xb[k]) + (yb[j] - yb[k])*(yb[j] - yb[k]) + (zb[j] - zb[k])*(zb[j] - zb[k]));
        }
    }
// calculating the distances between residue i and j (after)
  
    for (j=0;j<m;j++){
        for (k=0;k<m;k++){
            dista[j][k] = sqrt((xa[j] - xa[k])*(xa[j] - xa[k]) + (ya[j] - ya[k])*(ya[j] - ya[k]) + (za[j] - za[k])*(za[j] - za[k]));
        }
    }

// now calculating the difference of distances between before and after
    for (j=0;j<m;j++){
        for (k=0;k<m;k++){
            diff[j][k] = dista[j][k] - distb[j][k];
            if (diff[j][k] < 0)
                diff[j][k] = -diff[j][k];
        }
    }

// print out the results
    for (j=0;j<m;j++){
        for (k=0;k<m;k++){
            fprintf(fp, "%3d\t%3d\t%6.3lf\n", j+32, k+32, diff[j][k]);
        }
    }
    
  return 0;
}


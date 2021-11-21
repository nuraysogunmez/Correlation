#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


int main(int argn, char *arg[]) {
  
    FILE *fd, *fp;
    int i, j, k, m, nres, nconf;
    char line[1024], key[128], name[128];
    double sumx[400], sumy[400], sumz[400];
    double **ax, **ay, **az, *xb, *yb, *zb;
    double avgx[400], avgy[400], avgz[400];
    double **drx, **dry, **drz;
    double dridri[400], dridrj[400][400], corr[400][400];
 
    
    if (argn!=2) {
        fprintf(stderr, "Usage: %s <filename> <run>\n", arg[0]);
        return 1;
    }
 
// open for reading only
    fd = fopen(arg[1], "r");

    if (fd == NULL) {
        fprintf(stderr, "Cannot open %s.\n", arg[1]);
        return 1;
    }

    // open for writing only
    fp = fopen ("correlation.dat", "w");

// get the total number of lines
// qso we allocate (more than) enough space for x,y,z arrays
    m = 0;
    while (fgets(line, 1024, fd)) {
        m++;
    }
    printf("%d\n",m);
 
    nconf = 3750;
    nres = 311;
    k = nconf*nres;
    printf("%d %d %d\n",nconf, nres, k);

// allocate memory for tmp x, y, z coordinate arrays
    xb = malloc(sizeof(double)*k);
    yb = malloc(sizeof(double)*k);
    zb = malloc(sizeof(double)*k);
    
    ax = malloc(sizeof(double)*nconf);
    ay = malloc(sizeof(double)*nconf);
    az = malloc(sizeof(double)*nconf);
    
    for (i=0;i<nconf;i++) ax[i] = malloc(sizeof(double)*nres);
    for (i=0;i<nconf;i++) ay[i] = malloc(sizeof(double)*nres);
    for (i=0;i<nconf;i++) az[i] = malloc(sizeof(double)*nres);

    drx = malloc(sizeof(double)*nconf);
    dry = malloc(sizeof(double)*nconf);
    drz = malloc(sizeof(double)*nconf);
    
    for (i=0;i<nconf;i++) drx[i] = malloc(sizeof(double)*nres);
    for (i=0;i<nconf;i++) dry[i] = malloc(sizeof(double)*nres);
    for (i=0;i<nconf;i++) drz[i] = malloc(sizeof(double)*nres);

    
// rewind to the beginning of the file
    rewind(fd);
    
// now reading the coordinates for every conformation in the trajectory
    m = 0;
    while (fgets(line, 1024, fd)) {
        sscanf(line, "%s %*s %s %*s %*s %*s %lf %lf %lf %*s %*s", key, name, &(xb[m]), &(yb[m]), &(zb[m]));
        if (strcmp(key, "ATOM")==0) {
            if (strcmp(name, "CA")==0){
                m++;
            }
        }
    }
    
    printf("%d\n",m);
    
    m = 0;
    for (i=0;i<nconf;i++) {
        for (j=0;j<nres;j++) {
            ax[i][j] = xb[m];
            ay[i][j] = yb[m];
            az[i][j] = zb[m];
            //printf("%lf %lf %lf %lf %lf %lf %d %d\n",xb[m],yb[m],zb[m],ax[i][j],ay[i][j],az[i][j],i,j);
            m++;
        }  
    }

    printf("%d\n",m);
    
// calculating the average position (over N snapshots) of each alpha Carbon atom in residue i
    
    for (j=0;j<nres;j++){
        sumx[j] = 0.0;
        sumy[j] = 0.0;
        sumz[j] = 0.0;
        for (i=0;i<nconf;i++){
            sumx[j] = sumx[j] + ax[i][j];
            sumy[j] = sumy[j] + ay[i][j];
            sumz[j] = sumz[j] + az[i][j];
        }
        avgx[j] = sumx[j] / nconf;
        avgy[j] = sumy[j] / nconf;
        avgz[j] = sumz[j] / nconf;
        //printf("%d %lf %lf %lf\n",j, avgx[j], avgy[j], avgz[j]);
    }
    
// calculating the fluctuation vector from the average for each alpha Carbon of residue i for each snapshot (Delta Ri)
  
    for (i=0;i<nconf;i++) {
        for (j=0;j<nres;j++) {
            drx[i][j] = ax[i][j] - avgx[j];
            dry[i][j] = ay[i][j] - avgy[j];
            drz[i][j] = az[i][j] - avgz[j];
         }
    }

// now calculating the correlation between residue i and j ( <Delta Ri . Delta Rj> /(<Delta Ri . Delta Ri><Delta Rj . Delta Rj> )^1/2 )
    for (i=0;i<nres;i++) {
        dridri[i] = 0.0;
        for (k=0;k<nconf;k++)
            dridri[i] = dridri[i] + (drx[k][i] * drx[k][i] + dry[k][i] * dry[k][i] + drz[k][i] * drz[k][i]);
        for (j=0;j<nres;j++) {
            dridrj[i][j] = 0.0;
            for (k=0;k<nconf;k++)
                dridrj[i][j] = dridrj[i][j] + (drx[k][i] * drx[k][j] + dry[k][i] * dry[k][j] + drz[k][i] * drz[k][j]);
             //printf("%d %lf %lf\n",i,j,dridrj[i][j],sqrt(dridrj[i][j]));
        }
        //printf("%d %lf %lf\n",i,dridri[i],sqrt(dridri[i]));
    }
    
    for (i=0;i<nres;i++) {
        for (j=0;j<nres;j++)
            corr[i][j] = dridrj[i][j] / sqrt(dridri[i] * dridri[j]);
    }
    

// print out the results
    for (i=0;i<nres;i++) {
        for (j=0;j<nres;j++)
            fprintf(fp, "%5d %5d %8.4lf\n", i, j, corr[i][j]);
    }
    
  return 0;
}


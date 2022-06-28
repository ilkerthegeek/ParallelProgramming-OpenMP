	#include <stdio.h>
	#include <math.h>
	#include <omp.h>
	#include <stdlib.h>

	int main(int argc, char *argv[])
	{
	int m; 
	int n;
	double tol;// = 0.0001;

	int i, j, iter,nthreads;
    double tstart, tstop;
	m = atoi(argv[1]);
	n = atoi(argv[2]);
	tol = atof(argv[3]);
	nthreads = atof(argv[4]);

	double t[m+2][n+2], tnew[m+1][n+1], diff, difmax,priv_difmax;

	printf("%d %d %lf\n",m,n, tol);
	/* define the number of threads to be used */
    omp_set_num_threads(nthreads);
	

	tstart = omp_get_wtime ();
	// initialise temperature array
	#pragma omp parallel for schedule(static) \
    	default(shared) private(i, j)
	for (i=0; i <= m+1; i++) {
		for (j=0; j <= n+1; j++) {
			t[i][j] = 30.0;
		}
	}

	// fix boundary conditions
	for (i=1; i <= m; i++) {
		t[i][0] = 40.0;
		t[i][n+1] = 30.0;
	}
	
	for (j=1; j <= n; j++) {
		t[0][j] = 100.0;
		t[m+1][j] = 20.0;
	}

	// main loop
	iter = 0;
	difmax = 1000000.0;


		difmax = 0.0;
		// update temperature for next iteration
		#pragma omp parallel default(shared) private(i,j, diff, priv_difmax)
	 	{
			priv_difmax = 0.0;
			for (i=1; i <= m; i++) {
				for (j=1; j <= n; j++) {
                    tnew[i][j] = (t[i-1][j]+t[i+1][j]+t[i][j-1]+t[i][j+1])/4.0;

					diff = fabs(tnew[i][j]-t[i][j]);
					if (diff > difmax) {
						priv_difmax = diff;
					}
					// copy new to old temperatures
						t[i][j] = tnew[i][j];
				}
			}
			#pragma omp critical
        	if (priv_difmax > difmax) {
            	difmax = priv_difmax;
       		 }
		}

	

	tstop = omp_get_wtime ();

// print results
    
    for (i=0; i <= m+1; i++) {
	    printf("\n");
	    for (j=0; j <= n+1; j++) {
		    printf("%3.5lf ", t[i][j]);
	    }
    }
    printf("iter = %d  difmax = %9.11lf", iter, difmax);
    printf("\n");
	// I just changed microseconds to seconds with manipulating the seconds.Because omp_get_wtime returns seconds not microseconds.To compare easily with 
	// first questions microseconds.
	printf("time taken is %4.8lf\n", (tstop-tstart)*1000000);

}
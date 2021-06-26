#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

#define N 10
//jan 2021
/*
int main()
{
	int i;
	long long int t, a[N], b[N], c[N], a1[N];


	for (i = 0; i < N; i++)
	{
		a[i] = i + 1;
		b[i] = 2;
		c[i] = 3;
	}

	t = 1;
	for (i = 0; i < N - 1; i++)
	{
		a[i] = a[i + 1] + b[i] * c[i];
		t = t * a[i];
	}

	for (i = 0; i < N; i++)
		printf("%lld ", a[i]);
	printf("t= %lld \n", t);
	for (i = 0; i < N; i++)
	{
		a[i] = i + 1;
		a1[i] = i + 1;
		b[i] = 2;
		c[i] = 3;
	}

	t = 1;
	omp_set_num_threads(8);
#pragma omp parallel for reduction(*:t) private(i)
	for (i = 0; i < N - 1; i++)
	{
		a[i] = a1[i + 1] + b[i] * c[i];
		t *= a[i];
	}

	for (i = 0; i < N; i++)
		printf("%lld ", a[i]);
	printf("t= %lld \n", t);

}
//*/

//sept 2020
/*
int main()
{
	int m;
	int b[N + 1];
	int d = 0;
	int sum;

	for (m = 0; m < N + 1; m++)
		b[m] = m;

	for (m = 0; m < N; m++)
	{
		b[m] = b[m + 1] * d;
		d = d + m;
	}

	for (m = 0; m < N + 1; m++)
		printf("%d ", b[m]);
	printf("d = %d \n",d);

	d = 0;
	sum = 0;
	for (m = 0; m < N + 1; m++)
		b[m] = m;

#pragma omp parallel for reduction(+:sum) private(m)
	for (m = 0; m < N; m++)
	{
		b[m] = b[m + 1] * (d + (m * (m - 1)) / 2);
		sum += m;
	}
	d = sum;

	for (m = 0; m < N + 1; m++)
		printf("%d ", b[m]);
	printf("d = %d \n", d);

}
//*/

//okt 2020
/*
int main()
{
	int z[N];
	int k, x;

	for (k = 0; k < N; k++)
		z[k] = k;


	x = 1;
	for (k = 0; k < N; k++)
	{
		z[k] = k + x;
		x = k;
	}

	for (k = 0; k < N; k++)
		printf("%d ", z[k]);
	printf("x = %d\n", x);


	for (k = 0; k < N; k++)
		z[k] = k;
	omp_set_num_threads(5);
	x = 1;
	z[0] = x;//prva iteracija se izvuce ispred
	#pragma omp parallel for lastprivate(x) private(k)
	for (k = 1; k < N; k++)
	{
		z[k] = k + k-1; // a posto smo izvukli prvi slucaj, ostali se svode da se na k dodaje predhodno k, tj k - 1
		x = k; // za x nam je bitna samo posledjna vrednost iteracije
	}

	for (k = 0; k < N; k++)
		printf("%d ", z[k]);
	printf("x = %d\n", x);


}
//*/

//sept 2019
/*
int main()
{
	int a[N];

	srand(omp_get_wtime());
	for (int i = 0; i < N; i++)
		a[i] = i * i;

	int maks,i;
	maks = a[0];
	omp_set_num_threads(5);

	////izvrsava se paralelno
	#pragma omp parallel for reduction(max:maks)
	for (i = 0; i < N; i++)
	{
		maks = maks < a[i] ? a[i] : maks;
	}

	maks = a[0];
	////izvrsava se prakticno sekvencijalno
	//#pragma omp parallel for shared(maks)
	//for (i = 0; i < N; i++)
	//{
	//	#pragma omp critical
	//	maks = maks < a[i] ? a[i] : maks;
	//}

	printf("max je %d\n", maks);
}
//*/

//jun 2019
/*
int main()
{
	int b[N * N * 2 + 1], a[N];
	int id, i, j;

	for (i = 0; i < N; i++)
		a[i] = 0;

	for (i = 0; i < N*N*2 + 1; i++)
		b[i] = i;

	id = 2;
	for(int i =0;i<N;i++)
		for (int j = 0; j < N; j++)
		{
			a[j] += b[id];
			id += 2;
		}

	for (i = 0; i < N; i++)
		printf("%d ", a[i]);
	printf("id = %d\n", id);



	for (i = 0; i < N; i++)
		a[i] = 0;

	omp_set_num_threads(5);
	id = 2;
	#pragma omp parallel for reduction(+:id)
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			a[j] += b[i * N * 2 + j * 2 + 2];
			id += 2;
		}


	for (i = 0; i < N; i++)
		printf("%d ", a[i]);
	printf("id = %d\n", id);
}
//*/

//apr 2018
/*
int main()
{
	int a[N][N], b[N][N];
	int i, j, sum;
	float avg;

	for(i = 0 ; i < N;i++)
		for (j = 0; j < N; j++)
		{
			a[i][j] = i * N + j;
			b[i][j] = i + j;
		}

	sum = 0;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			sum += a[i][j];
		}
	}
	avg = (float)sum / (float)(N * N);

	printf("avg = %f\n", avg);

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			//a[i][j] = i * N + j;
			b[i][j] = i + j;
		}
	omp_set_num_threads(5);
	sum = 0;
	#pragma omp parallel for reduction(+:sum) private(i,j)
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			sum += a[i][j];
		}
	}
	avg = (float)sum / (float)(N * N);
	printf("avg = %f\n", avg);

	#pragma omp parallel for private(i,j)
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			b[i][j] = a[i][j] - avg;
		}
	}
}
//*/

//apr 2021
/*
int f(int v) { return v + 1; }
int main()
{
	int v, sum, i, step, start;
	start = 10;

	step = 2;
	v = start;
	sum = 0;
	for (i = 0; i < N; i++)
	{
		sum = sum + f(v);
		v = v + step;
	}
	printf("sum = %d, v = %d\n", sum, v);

	step = 2;
	v = start;
	sum = 0;
	omp_set_num_threads(8);

#pragma omp parallel for reduction(+:v,sum)
	for (i = 0; i < N; i++)
	{
		sum += f(start + i * step);
		v += step;
	}
	printf("sum = %d, v = %d\n", sum, v);

}
//*/

//jun 2020
/*
int main()
{
	int z, s, i, A[N], B[N*N*2], j, k;
	k = 2;
	for (int i = 0; i < N; i++)
		A[i] = 0;
	for (int i = 0; i < N * N * 2; i++)
		B[i] = i;

	s = 0;
	z = s;

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			A[j] += B[z];
			z += k;
		}

	for (i = 0; i < N; i++)
		printf("%d ", A[i]);
	printf("z=%d\n", z);
	
	for (int i = 0; i < N; i++)
		A[i] = 0;
	for (int i = 0; i < N * N * 2; i++)
		B[i] = i;

	s = 0;
	z = s;
	omp_set_num_threads(5);
#pragma omp parallel for reduction(+:z)
	for (i= 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			A[j] += B[i * N * k + j * k + s];
			z += k;
		}
	}


	for (i = 0; i < N; i++)
		printf("%d ", A[i]);
	printf("z=%d\n", z);
}
//*/

//dec 2020
/*
* //radi sigurno..
int main()
{
	int z,m,i,j,X[N],Y[N*N*2];
	
	m = 0;
	z = m;
#pragma omp parallel for reduce(+:z)
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			X[j] += Y[m + i * N * 2 + j * 2];
			z += 2;
		}
	}
}
//*/

//apr 2020
/*
int f(int v) { return v + 1; }
int main()
{
	int v, sum, i, step, start;
	start = 10;

	step = 2;
	v = start;
	sum = 0;
	for (i = 0; i < N; i++)
	{
		sum = sum + f(v);
		v = v + step;
	}
	printf("sum = %d, v = %d\n", sum, v);

	step = 2;
	v = start;
	sum = 0;
	omp_set_num_threads(5);

#pragma omp parallel for reduction(+:v,sum)
	for (i = 0; i < N; i++)
	{
		sum = sum + f(start + i * step);
		v += step;
	}
	printf("sum = %d, v = %d\n", sum, v);

}
//*/

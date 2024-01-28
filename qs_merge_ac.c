#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>

#define NN 512000000	
#define MAX_INT ((int)((unsigned int)(-1)>>1))

int valors[NN+1];
int valors2[NN+1];

typedef struct {
	int *vin;
	int ne;
}QSArgs;

typedef struct {
	int* vin;
	int n;
	int* vout;
}MergeArgs;

void qs(int *vin, int ne)
{
	int i,f,j;
	int pivot,vtmp,vfi;
	
	pivot = vin[0];
	i = 1;
	f = ne-1;
	vtmp = vin[i];
	
	while (i <= f){
		if (vtmp < pivot) {
			vin[i-1] = vtmp;
			i ++;
			vtmp = vin[i];
		}
		else {
			vfi = vin[f];
			vin[f] = vtmp;
			f --;
			vtmp = vfi;
		}
	}
	vin[i-1] = pivot;
	
	if (f>1) qs(vin,f);
	if (i < ne-1) qs(&vin[i],ne-f-1);
}

void merge2(int* vin, int n,int *vout)
{
	int vtmp;
	int i,j,posi,posj;
	posi = 0;
	posj = (n/2);
	for (i=0;i<n;i++){
		if (((posi < n/2) && (vin[posi] <= vin[posj])) || (posj >= n))
			vout[i] = vin[posi++];
		else if (posj < n)
			vout[i] = vin[posj++];
	}
}
void* qs_thread(void *arg)
{
	QSArgs* params = (QSArgs*)arg;
	qs(params->vin, params->ne);
	return NULL;
}

void* qs_merge_thread(void *arg)
{
	MergeArgs* params = (MergeArgs*)arg;
	merge2(params->vin, params->n, params->vout);
	return NULL;
}


int main(int nargs, char* args[]) {
    int ndades, i, m, parts;
    int *vin, *vout, *vtmp;
    long long sum = 0;

    ndades = atoi(args[1]);
    assert(ndades <= NN);

    parts = atoi(args[2]);
    if (parts < 2)
        assert("Han d'haver dues parts com a minim" == 0);
    if (ndades % parts)
        assert("N ha de ser divisible per parts" == 0);	
		pthread_t merge_thread_id[parts];
	QSArgs qs_args[parts];

	for (i = 0; i < ndades; i++)
        valors[i] = rand() % MAX_INT;

	
	for (i=0;i<parts;i++){

		qs_args[i].vin = &valors[i*(ndades/parts)];
		qs_args[i].ne = ndades/parts;
		pthread_create(&merge_thread_id[i], NULL, qs_thread, (void*)&qs_args[i]);

	}
	for (i=0;i<parts;i++){
		pthread_join(merge_thread_id[i], NULL);
	}
	
		
	// Merge de dos vectors. 1 de cada 2 threads uneix els dos vectors
	// A cada volta nomes treballen la meitat dels threads fins arribar
	// al thread principal
	
	vin = valors;
	vout = valors2;
	MergeArgs merge_args[parts];
	int nparts = 0;
	for (m = 2*(ndades/parts); m <= ndades; m *= 2){
	nparts=0;
		for (i = 0; i < ndades; i += m){
			merge_args[nparts].vin = &vin[i];
			merge_args[nparts].n = m;
			merge_args[nparts].vout = &vout[i];
			pthread_create(&merge_thread_id[nparts], NULL, qs_merge_thread, (void*)&merge_args[nparts]);
			nparts++;
		}			
		vtmp=vin;
		vin=vout;
		vout=vtmp;
		
		for (i=0;i<nparts;i++)
			pthread_join(merge_thread_id[i], NULL);
		
	}

	
/*  EXEMPLE ASSIGNACIO FEINA a 4 THREADS
  qs(&valors[0],ndades/4);      	//per a TH0
  qs(&valors[ndades/4],ndades/4); 	//per a TH1
  qs(&valors[ndades/2],ndades/4); 	//per a TH2
  qs(&valors[3*ndades/4],ndades/4);	//per a TH3 

  // sincro barrera

  merge2(&valors[0],ndades/2,&valors2[0]);	//per a TH0
  merge2(&valors[ndades/2],ndades/2,&valors2[ndades/2]); //per a TH2
  
  /sincro barrera
  
  merge2(valors2,N,valors); //per a TH0
  vin=valors;
 */ 

    for (i = 1; i < ndades; i++)
        assert(vin[i - 1] <= vin[i]);
    for (i = 0; i < ndades; i += 100)
        sum += vin[i];
    printf("validacio %lld \n", sum);

}


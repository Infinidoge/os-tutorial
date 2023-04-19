#ifndef VISUALISE_H_
#define VISUALISE_H_

enum SortingAlgorithms { BUBBLE, INSERTION, QUICK, MERGE };

void visualiser();

#define RAND_MAX 32767
int rand();

extern enum SortingAlgorithms algorithm;

#endif // VISUALISE_H_

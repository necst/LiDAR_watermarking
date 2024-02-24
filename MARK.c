//WATERMARK SUGLI SPAZI DESIDERATI IN BASE AGLI INDICI NEL FILE indexes
//FACCIO IN MODO DI SPOSTARE IL MINOR NUMERO DI PUNTI POSSIBILE -->
//SPOSTO DI 4.9cm I PUNTI DELLA PARTE OPPOSTA DEL VALORE DESIDERATO (No 5cm per i problemi coi float)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define XDIMENSION 1600
#define YDIMENSION 1600
#define ZDIMENSION 350
//Questi sono il numero di spazi che voglio marcare
#define XNUMBER 1600
#define YNUMBER 1600
#define ZNUMBER 350
#define DISPLACEMENT 0.050000

typedef struct {
    float x;
    float y;
    float z;
    float r;
} point;

typedef struct node {

    point* point;
    struct node* next;
} List;

typedef enum value {over, under};

typedef struct {

    int index;
    enum value value;
} indexesToMark;


int main() {

    FILE *currentFrame;
    int counter, numOfPoints, value, markingIndex, selectValue, i;
    float currentValue, currentMean;
    //I consider all the spaces with thickness 10cm (max relocation distance is 5cm)
    int Xindexes[XNUMBER];
    int Yindexes[YNUMBER];
    int Zindexes[ZNUMBER];
    time_t t;
    //Array of pointers of the head of the list of points in every space to mark
    List* Xspaces[XDIMENSION];
    List* Yspaces[YDIMENSION];
    List* Zspaces[ZDIMENSION];
    List* newNode;
    //List of bits representing the watermarking information
    int bits[XNUMBER+YNUMBER+ZNUMBER];
    int bitCounter;


    currentFrame = fopen("/Users/miche/Desktop/bits.bin", "wb+");

    if(currentFrame == NULL) {

        printf("\nCannot open bits file\n");
    }

    for (int j = 0; j < XNUMBER+YNUMBER+ZNUMBER; j++) {

        bits[j] = rand() % 2;
        fwrite(&bits[j], sizeof(bits[j]), 1, currentFrame);
        //printf("%d", bits[j]);
    }

    fclose(currentFrame);

    //INDEX VECTOR INITIALIZATION
    //The execution time of this phase is not considered in the watermarking execution time

    //I save the values of the indexes to mark in the corresponding array
    srand(time(&t));
    //printf("1\n");

    currentFrame = fopen("/Users/miche/Desktop/indexes.bin", "rb");

    for (counter = 0; counter < XNUMBER; counter++) {

        fread(&Xindexes[counter], sizeof(Xindexes[counter]), 1, currentFrame);
        //printf("\nIndiciX: %d", Xindexes[counter]);
        //Xindexes[counter] = counter;
    }

    if(counter < 1599) {

        fread(&i, sizeof(i), 1, currentFrame);
        counter++;
    }

    for (counter = 0; counter < YNUMBER; counter++) {

        fread(&Yindexes[counter], sizeof(Yindexes[counter]), 1, currentFrame);
        //Yindexes[counter] = counter;
    }

    if(counter < 1599) {

        fread(&i, sizeof(i), 1, currentFrame);
        counter++;
    }

    for (counter = 0; counter < ZNUMBER; counter++) {

        fread(&Zindexes[counter], sizeof(Zindexes[counter]), 1, currentFrame);
        //Zindexes[counter] = counter;
    }

    fclose(currentFrame);

    //printf("? %d\n", Xindexes[928].index);
    clock_t start = clock();

    //I put a NULL value in all the heads in the arrays
    for(counter = 0; counter < ZDIMENSION; counter++) {

        Xspaces[counter] = NULL;
        Yspaces[counter] = NULL;
        Zspaces[counter] = NULL;
    }
    //I'm assuming that XDIMENSION and YDIMENSION have the same value
    while(counter < XDIMENSION) {

        Xspaces[counter] = NULL;
        Yspaces[counter] = NULL;
        counter++;
    }

    //Opening the frame to be read
    //TODO: fare in modo che il file da aprire venga inserito come input nel programma dall'utente (argc, argv)
    //currentFrame = fopen("/Users/miche/Desktop/velodyne/007032.bin", "rb");
    currentFrame = fopen("/Users/miche/Desktop/velodyne/001578.bin", "rb");

    if(currentFrame == NULL) {

        printf("\nCannot open the file\n");
    }

    //Counting the number of points to correctly initialize the array

    //I put the pointer in the file end
    fseek(currentFrame, 0, SEEK_END);
    //I count the number of bytes in the file
    counter = ftell(currentFrame);
    //The number of points is counter/16 because we have 4 float (32bits) coordinates for every point
    counter = counter/16;
    //DEBUG:
    printf("\nHow many points? %d", counter);
    //I put the pointer in the beginning of the file
    rewind(currentFrame);

    //This is the array with all the points
    //printf("\narray");
    point points[counter];
    //printf("\narray");
    //This variable represents the number of points and will never be changed
    numOfPoints = counter;

    //START PREPROCESSING PHASE

    //Reading points in the file and copying them in the array
    value = 0;
    while(value != counter) {

        //printf("\nCounter: %d ", counter);
        //READ X VALUE
        fread(&points[value].x, sizeof(points[value].x), 1, currentFrame);
        //I select the correct space by dividing the x coord with the space thickness and adding half the max dimension (remind positive and negative values)
        markingIndex = (points[value].x/0.1) + (XDIMENSION/2);
        /*if(points[value].x < 0) {

            printf("\nVALORE: %f           INDEX: %d", points[value].x, markingIndex);
        }*/
        //printf("\n%d", markingIndex);
        if(Xspaces[markingIndex] == NULL) {

            /*if(markingIndex == 837) {

                printf("\nValore: %f", points[value].x);
            }*/
            Xspaces[markingIndex] = (List *) malloc(sizeof(List));
            Xspaces[markingIndex]->point = &points[value];
            Xspaces[markingIndex]->next = NULL;
        }
        else {

            newNode = (List *) malloc(sizeof(List));
            newNode->point = &points[value];
            newNode->next = Xspaces[markingIndex];
            Xspaces[markingIndex] = newNode;
        }

        //READ Y VALUE
        fread(&points[value].y, sizeof(points[value].y), 1, currentFrame);
        //I select the correct space by dividing the y coord with the space thickness and adding 800
        markingIndex = (points[value].y/0.1) + (YDIMENSION/2);
        if(Yspaces[markingIndex] == NULL) {

            Yspaces[markingIndex] = (List *) malloc(sizeof(List));
            Yspaces[markingIndex]->point = &points[value];
            Yspaces[markingIndex]->next = NULL;
        }
        else {

            newNode = (List *) malloc(sizeof(List));
            newNode->point = &points[value];
            newNode->next = Yspaces[markingIndex];
            Yspaces[markingIndex] = newNode;
        }
        //printf("Y: %f ", currentValue);

        fread(&points[value].z, sizeof(points[value].z), 1, currentFrame);
        //I select the correct space by dividing the x coord with the space thickness and adding 800
        markingIndex = (points[value].z/0.1) + (ZDIMENSION/2);
        if(Zspaces[markingIndex] == NULL) {

            Zspaces[markingIndex] = (List *) malloc(sizeof(List));
            Zspaces[markingIndex]->point = &points[value];
            Zspaces[markingIndex]->next = NULL;
        }
        else {

            newNode = (List *) malloc(sizeof(List));
            newNode->point = &points[value];
            newNode->next = Zspaces[markingIndex];
            Zspaces[markingIndex] = newNode;
        }
        //printf("Z: %f ", currentValue);
        //this is to read the reflectivity value that isn't needed for our scope
        fread(&points[value].r, sizeof(points[value].r), 1, currentFrame);
        //printf("     Counter: %d \n", counter);
        value++;
    }

    //END PREPROCESSING PHASE

    //I have finished to initialize the structures needed, so I don't have to read anymore and I can close the file
    fclose(currentFrame);
    //When I write results, I create a new file

    //START WATERMARKING PHASE

    /*
                 *****MARK ON X*****
    */

    bitCounter = 0;
    //I repeat the operation for all the values in the array of indexes of spaces to mark
    for(counter = 0; counter < XNUMBER; counter++) {

        //printf("\n\nXCycleX %d", counter);
        //This is the head of the list of points in the desired region
        newNode = Xspaces[Xindexes[counter]];

        //printf("      1 %d\n", Xindexes[counter]);
        //At first I do the mean calculus
        value = 0;
        currentValue = 0;
        while(newNode != NULL) {

            currentValue = currentValue + newNode->point->x;
            if(Xindexes[counter] == 1503) {

                printf("\n\n\n\nVALORE: %f\n", newNode->point->x);
            }
            value++;
            newNode = newNode->next;
        }
        /*if(Xindexes[counter] == 898) {
            printf("\nNumero punti = %d", value);
        }*/

        //printf("      2\n");
        //printf("\nXCurrent value: %f \nXValue: %d", currentValue, value);
        //I check if there are points in the space considered (else I proceed with no more computations)
        if(value != 0) {

            //TODO: check the calculus (see the warning)
            //This is now the mean
            currentValue = currentValue / value;
            //printf("\nXMean: %f", currentValue);

            //TODO: CHECK CONVERSION
            currentMean = (Xindexes[counter]) - (XDIMENSION/2);
            //printf("\nXCurrentMean: %f", currentMean);
            /*dopo questo passaggio ho un'imprecisione per via di piccoli decimali (0,00000x), non credo sia un grosso problema
                in quanto anche nella parte di estrazione del watermark ho la stessa situazione.
             Si tratta di un problema legato ai float
            */
            currentMean = (currentMean * 0.100000);
            //printf("\nXCurrentMean: %f", currentMean);
            currentMean = currentMean + 0.050000;

            //currentMean = (roundf(currentMean*10))/10;
            if(Xindexes[counter] == 1503) {

                printf("\nXAverage value: %f", currentMean);
                printf("\nValore medio (da modificare): %f", currentValue);
                printf("\nbits[bitCounter] = %d", bits[bitCounter]);
            }
            //printf("\nXAverage value: %f", currentMean);

            //I check if the values for the watermark are correct
            if (bits[bitCounter] == 1) {

                //I check if I have to do some changes in point positions due to the watermark insertion
                if (currentValue < currentMean) {

                    //printf("\nRegion X [ %f , %f ]", (currentMean - 0.05), (currentMean + 0.05));
                    //If I have to do changes, I check if I have to move the points 2,5cm or 5cm away
                    currentMean = currentMean + 0.0050000;
                    newNode = Xspaces[Xindexes[counter]];
                    while (newNode != NULL) {

                        if (newNode->point->x < currentMean) {

                            if(newNode->point->x < (currentMean-0.01)) {

                                newNode->point->x = newNode->point->x + DISPLACEMENT;
                            } else {

                                newNode->point->x = newNode->point->x + 0.02;
                            }
                        }
                        if(Xindexes[counter] == 1503) {

                            printf("\n\n\n\nVALORE: %f\n", newNode->point->x);
                        }
                        newNode = newNode->next;
                    }
                }
            } else {

                //I check if I have to do some changes in point positions due to the watermark insertion
                if (currentValue >= currentMean) {

                    //printf("\nRegion X [ %f , %f ]", (currentMean - 0.05), (currentMean + 0.05));
                    //printf("\nXModifico");
                    //If I have to do changes, I check if I have to move the points 2,5cm or 5cm away
                    currentMean = currentMean - 0.0050000;
                    newNode = Xspaces[Xindexes[counter]];
                    while (newNode != NULL) {

                        if (newNode->point->x >= currentMean) {

                            if(newNode->point->x > (currentMean + 0.01)) {

                                newNode->point->x = newNode->point->x - DISPLACEMENT;
                            } else {

                                newNode->point->x = newNode->point->x - 0.02;
                            }
                        }
                        if(Xindexes[counter] == 1503) {

                            printf("\n\n\n\nVALORE2: %f\n", newNode->point->x);
                        }
                        newNode = newNode->next;
                    }
                }
            }
            bitCounter++;
        }

    }

    /*
                 *****MARK ON Y*****
    */
    //I repeat the operation for all the values in the array of indexes of spaces to mark
    for(counter = 0; counter < YNUMBER; counter++) {

        //printf("\n\nCycleY %d", counter);
        //This is the head of the list of points in the desired region
        newNode = Yspaces[Yindexes[counter]];

        //At first I do the mean calculus
        value = 0;
        currentValue = 0;
        while(newNode != NULL) {

            currentValue = currentValue + newNode->point->y;
            /*if(Yindexes[counter] == 840) {

                printf("\n\n\n\nVALORE: %f\n", newNode->point->y);
            }*/
            value++;
            newNode = newNode->next;
        }
        /*if(Yindexes[counter] == 840) {
            printf("\nNumero punti = %d", value);
        }*/

        //printf("\nYCurrent value: %f \nYValue: %d", currentValue, value);
        //I check if there are points in the space considered (else I proceed with no more computations)
        if(value != 0) {

            //TODO: check the calculus (see the warning)
            //This is now the mean
            currentValue = currentValue / value;
            //printf("\nYMean: %f", currentValue);

            //TODO: CHECK CONVERSION
            currentMean = (Yindexes[counter]) - (YDIMENSION/2);
            //printf("\nYCurrentMean: %f", currentMean);
            /*dopo questo passaggio ho un'imprecisione per via di piccoli decimali (0,00000x), non credo sia un grosso problema
                in quanto anche nella parte di estrazione del watermark ho la stessa situazione.
             Si tratta di un problema legato ai float
            */
            currentMean = (currentMean * 0.100000);
            //printf("\nYCurrentMean: %f", currentMean);
            currentMean = currentMean + 0.050000;
            //currentMean = (roundf(currentMean*10))/10;
            //printf("\nYAverage value: %f", currentMean);
            /*if(Yindexes[counter] == 840) {

                printf("\nXAverage value: %f", currentMean);
                printf("\nValore medio (da modificare): %f", currentValue);
                printf("\nbits[bitCounter] = %d", bits[bitCounter]);
            }*/

            //I check if the values for the watermark are correct
            if (bits[bitCounter] == 1) {

                //I check if I have to do some changes in point positions due to the watermark insertion
                if (currentValue < currentMean) {

                    //printf("\nYModifico");
                    //printf("\nRegion Y [ %f , %f ]", (currentMean - 0.05), (currentMean + 0.05));
                    //If I have to do changes, I check if I have to move the points 2,5cm or 5cm away
                    currentMean = currentMean + 0.0050000;
                    newNode = Yspaces[Yindexes[counter]];
                    while (newNode != NULL) {

                        if (newNode->point->y < currentMean) {

                            if(newNode->point->y < (currentMean-0.01)) {

                                newNode->point->y = newNode->point->y + DISPLACEMENT;
                            } else {

                                newNode->point->y = newNode->point->y + 0.02;
                            }
                        }
                        newNode = newNode->next;
                    }
                }
            } else {

                //I check if I have to do some changes in point positions due to the watermark insertion
                if (currentValue >= currentMean) {

                    //printf("\nRegion CAMBIO Y [ %f , %f ]", (currentMean - 0.05), (currentMean + 0.05));
                    //printf("\nYModifico");
                    //If I have to do changes, I check if I have to move the points 2,5cm or 5cm away
                    currentMean = currentMean - 0.0050000;
                    newNode = Yspaces[Yindexes[counter]];
                    while (newNode != NULL) {

                        if (newNode->point->y >= currentMean) {

                            if(newNode->point->y > (currentMean + 0.01)) {

                                newNode->point->y = newNode->point->y - DISPLACEMENT;
                            } else {

                                newNode->point->y = newNode->point->y - 0.02;
                            }
                        }
                        newNode = newNode->next;
                    }
                }
            }
            bitCounter++;
        }
    }


    /*
                 *****MARK ON Z*****
    */
    //I repeat the operation for all the values in the array of indexes of spaces to mark
    for(counter = 0; counter < ZNUMBER; counter++) {

        //printf("\n\nCycleZ %d", counter);
        //This is the head of the list of points in the desired region
        newNode = Zspaces[Zindexes[counter]];

        //At first I do the mean calculus
        value = 0;
        currentValue = 0;
        while(newNode != NULL) {

            currentValue = currentValue + newNode->point->z;
            value++;
            newNode = newNode->next;
        }

        //printf("\nZCurrent value: %f \nZValue: %d", currentValue, value);
        //I check if there are points in the space considered (else I proceed with no more computations)
        if(value != 0) {

            //TODO: check the calculus (see the warning)
            //This is now the mean
            currentValue = currentValue / value;
            //printf("\nZMean: %f", currentValue);

            //TODO: CHECK CONVERSION
            currentMean = (Zindexes[counter]) - (ZDIMENSION/2);
            //printf("\nZCurrentMean: %f", currentMean);
            /*dopo questo passaggio ho un'imprecisione per via di piccoli decimali (0,00000x), non credo sia un grosso problema
                in quanto anche nella parte di estrazione del watermark ho la stessa situazione.
             Si tratta di un problema legato ai float
            */
            currentMean = (currentMean * 0.100000);
            //printf("\nZCurrentMean: %f", currentMean);
            currentMean = currentMean + 0.050000;
            //currentMean = roundf(currentMean);
            //printf("\nZAverage value: %f", currentMean);

            //I check if the values for the watermark are correct
            if (bits[bitCounter] == 1) {

                //I check if I have to do some changes in point positions due to the watermark insertion
                if (currentValue < currentMean) {

                    //printf("\nRegion Z [ %f , %f ]", (currentMean - 0.05), (currentMean + 0.05));
                    //printf("\nZModifico");
                    //If I have to do changes, I check if I have to move the points 2,5cm or 5cm away
                    currentMean = currentMean + 0.0050000;
                    newNode = Zspaces[Zindexes[counter]];
                    while (newNode != NULL) {

                        if (newNode->point->z < currentMean) {

                            if(newNode->point->z < (currentMean-0.01)) {

                                newNode->point->z = newNode->point->z + DISPLACEMENT;
                            } else {

                                newNode->point->z = newNode->point->z + 0.02;
                            }
                        }
                        newNode = newNode->next;
                    }
                }
            } else {

                //I check if I have to do some changes in point positions due to the watermark insertion
                if (currentValue >= currentMean) {

                    //printf("\nRegion Z [ %f , %f ]", (currentMean - 0.05), (currentMean + 0.05));
                    //printf("\nZModifico");
                    //If I have to do changes, I check if I have to move the points 2,5cm or 5cm away
                    currentMean = currentMean - 0.0050000;
                    newNode = Zspaces[Zindexes[counter]];
                    while (newNode != NULL) {

                        if (newNode->point->z >= currentMean) {

                            if(newNode->point->z > (currentMean + 0.01)) {

                                newNode->point->z = newNode->point->z - DISPLACEMENT;
                            } else {

                                newNode->point->z = newNode->point->z - 0.02;
                            }
                        }
                        newNode = newNode->next;
                    }
                }
            }
            bitCounter++;
        }
    }

    //END WATERMARKING PHASE


    //START WRITE-BACK PHASE

    //Open the file where to write results
    //printf("\nWrite back");
    currentFrame = fopen("/Users/miche/Desktop/results.bin", "wb+");
    if(currentFrame == NULL) {

        printf("\nCannot open the file\n");
    }

    //printf("8\n");
    for(counter = 0; counter < numOfPoints; counter++) {

        fwrite(&points[counter].x, sizeof(points[counter].x), 1, currentFrame);
        fwrite(&points[counter].y, sizeof(points[counter].y), 1, currentFrame);
        fwrite(&points[counter].z, sizeof(points[counter].z), 1, currentFrame);
        fwrite(&points[counter].r, sizeof(points[counter].r), 1, currentFrame);
    }

    fseek(currentFrame, 0, SEEK_SET);

    //test visivo per verificare che scrivo i valori corretti nel file
    /*
    value = 0;
    for(counter = 0; counter < numOfPoints; counter++) {

        if(value != 100) {
            fread(&currentValue, sizeof(currentValue), 1, currentFrame);
            fread(&currentValue, sizeof(currentValue), 1, currentFrame);
            fread(&currentValue, sizeof(currentValue), 1, currentFrame);
            fread(&currentValue, sizeof(currentValue), 1, currentFrame);
            value++;
        }
        else {

            fread(&currentValue, sizeof(currentValue), 1, currentFrame);
            printf("\nX: %f ", currentValue);
            printf("  X: %f ", points[counter].x);
            fread(&currentValue, sizeof(currentValue), 1, currentFrame);
            printf("\nY: %f ", currentValue);
            printf("  Y: %f ", points[counter].y);
            fread(&currentValue, sizeof(currentValue), 1, currentFrame);
            printf("\nZ: %f ", currentValue);
            printf("  Z: %f ", points[counter].z);
            fread(&currentValue, sizeof(currentValue), 1, currentFrame);
            printf("\nR: %f ", currentValue);
            printf("  R: %f \n", points[counter].r);
            value = 0;
        }
    }
    */

    //TEST IF I WRITE CORRECT VALUES IN THE FILE
    /*for(counter = 0; counter < numOfPoints; counter++) {

        fread(&currentValue, sizeof(currentValue), 1, currentFrame);
        if(currentValue != points[counter].x) {

            printf("\nERROR IN WRITE-BACK PHASE!!\n");
        }
        fread(&currentValue, sizeof(currentValue), 1, currentFrame);
        if(currentValue != points[counter].y) {

            printf("\nERROR IN WRITE-BACK PHASE!!\n");
        }
        fread(&currentValue, sizeof(currentValue), 1, currentFrame);
        if(currentValue != points[counter].z) {

            printf("\nERROR IN WRITE-BACK PHASE!!\n");
        }
        fread(&currentValue, sizeof(currentValue), 1, currentFrame);
        if(currentValue != points[counter].r) {

            printf("\nERROR IN WRITE-BACK PHASE!!\n");
        }
    }*/

    /*
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("\nX: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("Y: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("Z: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("R: %f \n", currentValue);
    printf("ULTIMO VALORE:  ");
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("X: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("Y: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("Z: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("R: %f \n", currentValue);

    //printf("\nErrore? %d", value);
    fseek(currentFrame, 0, SEEK_SET);
    value = fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    //printf("\nErrore? %d", value);
    printf("X: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("Y: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("Z: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("R: %f \n", currentValue);
    value = fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    //printf("\nErrore? %d", value);
    printf("X: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("Y: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("Z: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("R: %f ", currentValue);


    fseek(currentFrame, 0, SEEK_END);
    value = fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    //printf("\nErrore? %d", value);
    printf("X: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("Y: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("Z: %f ", currentValue);
    fread(&currentValue, sizeof(currentValue), 1, currentFrame);
    printf("R: %f ", currentValue);
    */

    fclose(currentFrame);

    //END WRITE-BACK PHASE
    /*
	//print last point
    printf("\n\nX: %f ", points[counter-2].x);
    printf("Y: %f ", points[counter-2].y);
    printf("Z: %f ", points[counter-2].z);
    printf("R: %f ", points[counter-2].r);
	printf("\nLAST POINT   ");
	printf("X: %f ", points[counter-1].x);
	printf("Y: %f ", points[counter-1].y);
	printf("Z: %f ", points[counter-1].z);
    printf("R: %f ", points[counter-1].r);


    //print first point
	printf("\nFIRST POINT   ");
	printf("X: %f ", points[0].x);
	printf("Y: %f ", points[0].y);
	printf("Z: %f ", points[0].z);
    printf("R: %f ", points[0].r);
    printf("\nSECOND POINT   ");
    printf("X: %f ", points[1].x);
    printf("Y: %f ", points[1].y);
    printf("Z: %f ", points[1].z);
    printf("R: %f ", points[1].r);
    */

    //newNode = Xspaces[837];
    //counter = 0;
    /*
    while(NULL != newNode) {

        printf("\nX = %f", (newNode->point)->x);
        newNode = newNode->next;
        counter++;
    }
    newNode = Yspaces[837];
    counter = 0;
    while(NULL != newNode) {

        printf("\nY = %f", (newNode->point)->y);
        newNode = newNode->next;
        counter++;
    }
    newNode = Yspaces[9];
    counter = 0;
    while(NULL != newNode) {

        printf("\nZ = %f", (newNode->point)->z);
        newNode = newNode->next;
        counter++;
    }
    */

    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("\nTime elapsed: %.5f\n", elapsed);
    printf("\nDisplacement: %f", DISPLACEMENT);
    //printf("\nNumero di punti: %d", counter);
    printf("\nFINITO");

    return(0);
}









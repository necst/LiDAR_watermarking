//CODICE PER ESTRARRE IL WATERMARK INSERITO (e verificare un eventuale tampering)

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


    currentFrame = fopen("/Users/miche/Desktop/bits.bin", "rb");

    if(currentFrame == NULL) {

        printf("\nCannot open bits file\n");
    }

    //read bits from the file and put them in the corresponding array
    for (int j = 0; j < XNUMBER+YNUMBER+ZNUMBER; j++) {

        fread(&bits[j], sizeof(bits[j]), 1, currentFrame);
        //printf("%d", bits[j]);
    }

    fclose(currentFrame);

    //INDEX VECTOR INITIALIZATION
    //The execution time of this phase is not considered in the watermarking execution time

    //This is a simple initialization just to test the following part of code
    srand(time(&t));

    currentFrame = fopen("/Users/miche/Desktop/indexes.bin", "rb");

    for (counter = 0; counter < XNUMBER; counter++) {

        fread(&Xindexes[counter], sizeof(Xindexes[counter]), 1, currentFrame);
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
    currentFrame = fopen("/Users/miche/Desktop/results.bin", "rb");

    if(currentFrame == NULL) {

        printf("\nCannot open the results file\n");
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

    //START WATERMARKING EXTRACTION PHASE

    /*
                 *****MARK ON X*****
    */

    bitCounter = 0;
    //I repeat the operation for all the values in the array of indexes of spaces to extract
    for(counter = 0; counter < XNUMBER; counter++) {

        //printf("\n\nXCycleX %d", counter);
        //This is the head of the list of points in the desired region
        newNode = Xspaces[Xindexes[counter]];

        //printf("      1 %d\n", Xindexes[counter].index);
        //At first I do the mean calculus
        value = 0;
        currentValue = 0;
        while(newNode != NULL) {

            currentValue = currentValue + newNode->point->x;
            /*if(Xindexes[counter] == 898) {

                printf("\n\n\n\nVALORE: %f\n", newNode->point->x);
            }*/
            value++;
            newNode = newNode->next;
        }
        /*if(Xindexes[counter] == 898) {
            printf("\nNumero punti = %d", value);
        }*/



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
            //currentMean = roundf(currentMean);
            //printf("\nXAverage value: %f", currentMean);

            //I check if the values for the watermark are correct
            if (bits[bitCounter] == 1) {

                //I check if I the marked value corresponds to the saved one and in negative case, I print the tampered region
                if (currentValue < currentMean) {

                    //In this case they do not correspond, I print out the tampered region
                    //printf("\nXModifico");

                    printf("\nTAMPERING DETECTED!   Region X [ %f , %f ]   CurrentValue=%f   valore letto: %d", (currentMean - 0.05), (currentMean + 0.05), currentValue, bits[bitCounter]);
                    printf("\nXindexes[counter] = %d", Xindexes[counter]);
                }
            } else {

                //I check if the values for the watermark are correct
                if (currentValue >= currentMean) {

                    //In this case they do not correspond, I print out the tampered region
                    printf("\nTAMPERING DETECTED!   Region X [ %f , %f ]   CurrentValue=%f   valore letto: %d", (currentMean - 0.05), (currentMean + 0.05), currentValue, bits[bitCounter]);
                    printf("\nXindexes[counter] = %d", Xindexes[counter]);
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
            /*if(Yindexes[counter] == 521) {

                printf("\n\n\n\nVALORE: %f\n", newNode->point->y);
            }*/
            value++;
            newNode = newNode->next;
        }

        if(Yindexes[counter] == 840) {
            printf("\nNumero punti = %d", value);
        }
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
            //currentMean = roundf(currentMean);
            //printf("\nYAverage value: %f", currentMean);

            //I check if the values for the watermark are correct
            if (bits[bitCounter] == 1) {

                //I check if I have to do some changes in point positions due to the watermark insertion
                if (currentValue < currentMean) {

                    //printf("\nYModifico");
                    //In this case they do not correspond, I print out the tampered region
                    printf("\nTAMPERING DETECTED!   Region Y [ %f , %f ]   CurrentValue=%f   valore letto: %d", (currentMean - 0.05), (currentMean + 0.05), currentValue, bits[bitCounter]);
                    printf("\nYindexes[counter] = %d", Yindexes[counter]);                }
            } else {

                //I check if I have to do some changes in point positions due to the watermark insertion
                if (currentValue >= currentMean) {

                    //printf("\nYModifico");
                    //In this case they do not correspond, I print out the tampered region
                    printf("\nTAMPERING DETECTED!   Region Y [ %f , %f ]   CurrentValue=%f   valore letto: %d", (currentMean - 0.05), (currentMean + 0.05), currentValue, bits[bitCounter]);
                    printf("\nYindexes[counter] = %d", Yindexes[counter]);                   }
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

                    //printf("\nZModifico");
                    //In this case they do not correspond, I print out the tampered region
                    printf("\nTAMPERING DETECTED!   Region Z [ %f , %f ]", (currentMean - 0.05), (currentMean + 0.05));
                }
            } else {

                //I check if I have to do some changes in point positions due to the watermark insertion
                if (currentValue >= currentMean) {

                    //printf("\nZModifico");
                    //In this case they do not correspond, I print out the tampered region
                    printf("\nTAMPERING DETECTED!   Region Z [ %f , %f ]", (currentMean - 0.05), (currentMean + 0.05));
                }
            }
            bitCounter++;
        }
    }

    //END WATERMARKING CHECK PHASE

    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("\nTime elapsed: %.5f\n", elapsed);
    //printf("\nNumero di punti: %d", counter);
    printf("\nFINITO");

    return(0);
}







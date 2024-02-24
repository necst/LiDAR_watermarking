//PRENDO LA SCANSIONE MARCATA E SPOSTO I PUNTI CORRISPONDENTI ALL'OGGETTO TROVATO (LO DUPLICO)

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
#define VARIAZIONE 100

#define SELECTED_POINTS 2000

#define PI 3.14159

typedef struct {
    float x;
    float y;
    float z;
    float r;
} point;

typedef struct {

    float tot;
    int num;
    float old_sum;
    int old_num;
} sum;

typedef struct node {

    point point;
    struct node* next;
} List;

int main() {

    FILE *markedScan, *tamperedScan;
    int counter, terminate, i;
    float x, y, z, l, w, h, r;
    float pointX, pointY, pointZ, pointR;
    float maxX, maxY, minX, minY, currX, currY;
    float angle, angCoeff1, angCoeff2, d, radius;
    float Xdis, Ydis, Zdis;
    int end;

    FILE *currentFrame;
    int numOfPoints, value, markingIndex, selectValue;
    float currentValue, currentMean;
    //I consider all the spaces with thickness 10cm (max relocation distance is 5cm)
    time_t t;
    //Array of pointers of the head of the list of points in every space to mark
    //List of bits representing the watermarking information
    int bits[XNUMBER + YNUMBER + ZNUMBER];
    int bitCounter;
    float counterX, counterY, counterZ;
    int detected, success, unsuccess, total;
    int tamperingX, tamperingY;
    int freeCounter;
    int Xindexes[XNUMBER];
    int Yindexes[YNUMBER];
    int Zindexes[ZNUMBER];
    List *newNode = NULL;
    sum Xspaces[XDIMENSION];
    sum Yspaces[YDIMENSION];
    sum Zspaces[ZDIMENSION];
    List *headSelected = NULL;
    int selectedCounter = 0;

    total = 0;
    success = 0;
    unsuccess = 0;

    //Values initialization
    //Initialization of coordinates of the first point
    printf("\nInsert X value: ");
    scanf("%f", &x);
    printf("\nInsert Y value: ");
    scanf("%f", &y);
    printf("\nY value: %f", y);
    printf("\nInsert Z value: ");
    scanf("%f", &z);
    printf("\nZ value: %f", z);
    //Inizialization of the value of the lenght of the box
    printf("\nInsert L value: ");
    scanf("%f", &l);
    //Inizialization of the value of the width (spessore) of the box
    printf("\nInsert W value: ");
    scanf("%f", &w);
    //Inizialization of the value of the height of the box
    printf("\nInsert H value: ");
    scanf("%f", &h);
    //Inizialization of the value of the rotation of the box
    printf("\nInsert R value: ");
    scanf("%f", &r);

    d = sqrt(pow(l, 2) + pow(w, 2)) / 2;
    radius = sqrt(pow(d, 2) + pow(h / 2, 2));

    currentFrame = fopen("/Users/miche/Desktop/bits.bin", "rb");

    if (currentFrame == NULL) {

        printf("\nCannot open bits file\n");
    }

    //read bits from the file and put them in the corresponding array
    for (int j = 0; j < XNUMBER + YNUMBER + ZNUMBER; j++) {

        fread(&bits[j], sizeof(bits[j]), 1, currentFrame);
        //printf("%d", bits[j]);
    }

    fclose(currentFrame);

    markedScan = fopen("/Users/miche/Desktop/results.bin", "rb");

    if (markedScan == NULL) {

        printf("\nCannot open the results file\n");
    }

    //Counting the number of points in the original scan
    //I put the pointer in the file end
    fseek(markedScan, 0, SEEK_END);
    //I count the number of bytes in the file
    counter = ftell(markedScan);
    //The number of points is counter/16 because we have 4 float (32bits) coordinates for every point
    counter = counter / 16;
    //DEBUG:
    printf("\nHow many points in the original scan? %d", counter);
    //I put the pointer in the beginning of the file
    rewind(markedScan);

    value = 0;

    while (value != counter) {

        //printf("\nCounter: %d ", counter);
        //READ X VALUE
        fread(&pointX, sizeof(pointX), 1, currentFrame);
        fread(&pointY, sizeof(pointY), 1, currentFrame);
        fread(&pointZ, sizeof(pointZ), 1, currentFrame);
        fread(&pointR, sizeof(pointR), 1, currentFrame);

        //I select the correct space by dividing the x coord with the space thickness and adding half the max dimension (remind positive and negative values)
        markingIndex = (pointX / 0.1) + (XDIMENSION / 2);

        Xspaces[markingIndex].tot = Xspaces[markingIndex].tot + pointX;
        Xspaces[markingIndex].old_sum = Xspaces[markingIndex].old_sum + pointX;
        Xspaces[markingIndex].old_num++;
        Xspaces[markingIndex].num++;

        //I select the correct space by dividing the y coord with the space thickness and adding 800
        markingIndex = (pointY / 0.1) + (YDIMENSION / 2);

        Yspaces[markingIndex].tot = Yspaces[markingIndex].tot + pointY;
        Yspaces[markingIndex].num++;
        Yspaces[markingIndex].old_sum = Yspaces[markingIndex].old_sum + pointY;
        Yspaces[markingIndex].old_num++;


        //printf("Y: %f ", currentValue);
        //I select the correct space by dividing the x coord with the space thickness and adding 800
        markingIndex = (pointZ / 0.1) + (ZDIMENSION / 2);
        Zspaces[markingIndex].tot = Zspaces[markingIndex].tot + pointZ;
        Zspaces[markingIndex].num++;
        Zspaces[markingIndex].old_sum = Zspaces[markingIndex].old_sum + pointZ;
        Zspaces[markingIndex].old_num++;
        value++;

        if ((pow((pointX - x), 2) + pow((pointY - y), 2) + pow((pointZ - z), 2)) <= pow(radius, 2)) {

            //Otherwise the point isn't in the box
            //printf("\nTrovato un punto");
            if (headSelected == NULL) {

                headSelected = (List *) malloc(sizeof(List));
                (headSelected->point).x = pointX;
                (headSelected->point).y = pointY;
                (headSelected->point).z = pointZ;
                (headSelected->point).r = pointR;
                headSelected->next = NULL;
            } else {

                newNode = (List *) malloc(sizeof(List));
                (newNode->point).x = pointX;
                (newNode->point).y = pointY;
                (newNode->point).z = pointZ;
                (newNode->point).r = pointR;
                newNode->next = headSelected;
                headSelected = newNode;
            }

        }
    }

    currentFrame = fopen("/Users/miche/Desktop/indexes.bin", "rb");

    for (counter = 0; counter < XNUMBER; counter++) {

        fread(&Xindexes[counter], sizeof(Xindexes[counter]), 1, currentFrame);
        //printf("\nIndiciX: %d", Xindexes[counter]);
        //Xindexes[counter] = counter;
    }

    if (counter < 1599) {

        fread(&i, sizeof(i), 1, currentFrame);
        counter++;
    }

    for (counter = 0; counter < YNUMBER; counter++) {

        fread(&Yindexes[counter], sizeof(Yindexes[counter]), 1, currentFrame);
        //Yindexes[counter] = counter;
    }

    if (counter < 1599) {

        fread(&i, sizeof(i), 1, currentFrame);
        counter++;
    }

    for (counter = 0; counter < ZNUMBER; counter++) {

        fread(&Zindexes[counter], sizeof(Zindexes[counter]), 1, currentFrame);
        //Zindexes[counter] = counter;
    }

    fclose(currentFrame);


    //------------------------------------------------------------------------------------
    printf("\nEND PREPROCESSING PHASE");

    counterX = -1;
    counterY = 0;
    counterZ = 0;

    int primo = 0;

    while(counterX + x > -78) {

        tamperingX = 0;
        while(counterY + y > -78) {

            tamperingY = 0;
            while(counterZ + z < 30) {

                //printf("\nEntro nel whileZ");
                if((tamperingX == 0) && (tamperingY == 0)) {

                    //printf("\nEntro nell'ifZ");
                    detected = 0;

                    if(counterX + x - 1 < -78) {

                        primo = 0;
                    }

                    selectedCounter = 0;
                    newNode = headSelected;
                    //printf("\nSono prima del whileZ");
                    while(newNode != NULL) {

                        //printf("\nEntro nel secondo whileZ, %f", newNode->point.x);
                        pointX = newNode->point.x + counterX;
                        pointY = newNode->point.y + counterY;
                        pointZ = newNode->point.z + counterZ;
                        pointR = newNode->point.r;

                        markingIndex = (pointX / 0.1) + (XDIMENSION / 2);

                        Xspaces[markingIndex].tot = Xspaces[markingIndex].tot + pointX;
                        Xspaces[markingIndex].num++;

                        markingIndex = (pointY / 0.1) + (YDIMENSION / 2);

                        Yspaces[markingIndex].tot = Yspaces[markingIndex].tot + pointY;
                        Yspaces[markingIndex].num++;

                        //printf("Y: %f ", currentValue);
                        //I select the correct space by dividing the x coord with the space thickness and adding 800
                        markingIndex = (pointZ / 0.1) + (ZDIMENSION / 2);
                        Zspaces[markingIndex].tot = Zspaces[markingIndex].tot + pointZ;
                        Zspaces[markingIndex].num++;

                        newNode = newNode->next;
                        if(newNode == NULL) {

                            //printf("\nSTOP");
                        }
                        //Yspaces[1277].tot = 1528;
                        //printf("\nMedia selezionata: %f", Xspaces[418].tot / Xspaces[418].num);
                    }

                    //printf("\nSTART WATERMARKING EXTRACTION PHASE");

                    /*
                                 *****EXTRACTION ON X*****
                    */

                    //printf("\nSTART EXTRACTION");
                    bitCounter = 0;
                    //I repeat the operation for all the values in the array of indexes of spaces to extract
                    counter = 0;
                    while ((counter < XNUMBER) && (detected != 1)) {

                        currentValue = Xspaces[Xindexes[counter]].tot;
                        value = Xspaces[Xindexes[counter]].num;

                        //I check if there are points in the space considered (else I proceed with no more computations)
                        if (value != 0) {

                            //TODO: check the calculus (see the warning)
                            //This is now the mean
                            currentValue = currentValue / value;
                            //printf("\nXindexes[counter]: %d          bits[bitCounter]: %d       counter = %d", Xindexes[counter], bits[bitCounter], counter);

                            //TODO: CHECK CONVERSION
                            currentMean = (Xindexes[counter]) - (XDIMENSION / 2);
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
                                    detected = 1;
                                    tamperingX = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region X [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nXindexes[counter] = %d", Xindexes[counter]);
                                }
                            } else {

                                //I check if the values for the watermark are correct
                                if (currentValue >= currentMean) {

                                    detected = 1;
                                    tamperingX = 1;
                                    //In this case they do not correspond, I print out the tampered region
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region X [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nXindexes[counter] = %d", Xindexes[counter]);
                                }
                            }
                            bitCounter++;
                        }
                        counter++;
                        if(primo == 0) {

                            detected = 0;
                        }
                    }
                    //printf("\ntotalDetectedX: %d", selectedCounter);


                    /*
                                 *****MARK ON Y*****
                    */

                    //I repeat the operation for all the values in the array of indexes of spaces to extract
                    counter = 0;
                    while ((counter < YNUMBER) && (detected != 1)) {

                        value = 0;
                        currentValue = 0;

                        currentValue = Yspaces[Yindexes[counter]].tot;
                        value = Yspaces[Yindexes[counter]].num;

                        //I check if there are points in the space considered (else I proceed with no more computations)
                        if (value != 0) {

                            //TODO: check the calculus (see the warning)
                            //This is now the mean
                            currentValue = currentValue / value;
                            //printf("\nXMean: %f", currentValue);

                            //TODO: CHECK CONVERSION
                            currentMean = (Yindexes[counter]) - (YDIMENSION / 2);
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
                                    detected = 1;
                                    tamperingY = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Y [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nYindexes[counter] = %d", Yindexes[counter]);*/
                                }
                            } else {

                                //I check if the values for the watermark are correct
                                if (currentValue >= currentMean) {

                                    //In this case they do not correspond, I print out the tampered region
                                    detected = 1;
                                    tamperingY = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Y [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nYindexes[counter] = %d", Yindexes[counter]);*/
                                }
                            }
                            bitCounter++;
                        }
                        counter++;
                        if(primo == 0) {

                            detected = 0;
                        }
                    }
                    //printf("\ntotalDetectedY: %d", selectedCounter);



                    /*
                                 *****MARK ON Z*****
                    */
                    //printf("\nZ extraction");
                    //I repeat the operation for all the values in the array of indexes of spaces to mark
                    counter = 0;
                    while ((counter < ZNUMBER) && (detected != 1)) {

                        value = 0;
                        currentValue = 0;

                        currentValue = Zspaces[Zindexes[counter]].tot;
                        value = Zspaces[Zindexes[counter]].num;

                        //printf("\nZCurrent value: %f \nZValue: %d", currentValue, value);
                        //I check if there are points in the space considered (else I proceed with no more computations)
                        if (value != 0) {

                            //TODO: check the calculus (see the warning)
                            //This is now the mean
                            currentValue = currentValue / value;
                            //printf("\nZMean: %f", currentValue);

                            //TODO: CHECK CONVERSION
                            currentMean = (Zindexes[counter]) - (ZDIMENSION / 2);
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
                                    detected = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Z [ %f , %f ]    CounterX: %f    CounterY: %f    CounterZ: %f", (currentMean - 0.05),
                                           (currentMean + 0.05), counterX, counterY, counterZ);
                                }
                            } else {

                                //I check if I have to do some changes in point positions due to the watermark insertion
                                if (currentValue >= currentMean) {

                                    //printf("\nZModifico");
                                    //In this case they do not correspond, I print out the tampered region
                                    detected = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Z [ %f , %f ]    CounterX: %f    CounterY: %f    CounterZ: %f", (currentMean - 0.05),
                                           (currentMean + 0.05), counterX, counterY, counterZ);
                                }
                            }
                            bitCounter++;
                        }
                        counter++;
                        if(primo == 0) {

                            detected = 0;
                        }
                    }
                    //printf("\ntotalDetectedZ: %d", selectedCounter);


                    primo = 1;

                    if (detected == 1) {

                        success++;
                    } else {

                        printf("\n\nTAMPERING NOT DETECTED!   CounterX: %f    CounterY: %f    CounterZ: %f\n\n", counterX, counterY, counterZ);
                        unsuccess++;
                    }
                    total++;

                    printf("\ntotalDetected: %d", selectedCounter);

                    for(i = 0; i < ZDIMENSION; i++) {

                        Xspaces[i].tot = Xspaces[i].old_sum;
                        Xspaces[i].num = Xspaces[i].old_num;
                        Yspaces[i].tot = Yspaces[i].old_sum;
                        Yspaces[i].num = Yspaces[i].old_num;
                        Zspaces[i].tot = Zspaces[i].old_sum;
                        Zspaces[i].num = Zspaces[i].old_num;
                    }

                    //printf("\nOldSum: %f", Yspaces[61].old_sum / Yspaces[61].old_num);

                    while (i < XDIMENSION) {

                        Xspaces[i].tot = Xspaces[i].old_sum;
                        Xspaces[i].num = Xspaces[i].old_num;
                        Yspaces[i].tot = Yspaces[i].old_sum;
                        Yspaces[i].num = Yspaces[i].old_num;
                        Zspaces[i].tot = Zspaces[i].old_sum;
                        Zspaces[i].num = Zspaces[i].old_num;
                        i++;
                    }



                } else {

                    total++;
                    success++;
                }
                counterZ = counterZ + 1;
            }
            counterZ = 0;
            //printf("\nCounterY: %f", counterY);
            counterY = counterY - 1;
        }
        counterY = 0;
        counterX = counterX - 1;
        //printf("\nCounterX: %f", counterX);
    }
    //END WATERMARKING CHECK PHASE
    //printf("\nFINITO ESTRAZIONE");

    counterX = -1;
    counterY = 1;
    counterZ = 0;

    while(counterX + x > -78) {

        tamperingX = 0;
        while(counterY + y < 78) {

            tamperingY = 0;
            while(counterZ + z < 30) {

                //printf("\nEntro nel whileZ");
                if((tamperingX == 0) && (tamperingY == 0)) {

                    //printf("\nEntro nell'ifZ");
                    detected = 0;


                    selectedCounter = 0;
                    newNode = headSelected;
                    //printf("\nSono prima del whileZ");
                    while(newNode != NULL) {

                        //printf("\nEntro nel secondo whileZ, %f", newNode->point.x);
                        pointX = newNode->point.x + counterX;
                        pointY = newNode->point.y + counterY;
                        pointZ = newNode->point.z + counterZ;
                        pointR = newNode->point.r;

                        markingIndex = (pointX / 0.1) + (XDIMENSION / 2);

                        Xspaces[markingIndex].tot = Xspaces[markingIndex].tot + pointX;
                        Xspaces[markingIndex].num++;

                        markingIndex = (pointY / 0.1) + (YDIMENSION / 2);

                        Yspaces[markingIndex].tot = Yspaces[markingIndex].tot + pointY;
                        Yspaces[markingIndex].num++;

                        //printf("Y: %f ", currentValue);
                        //I select the correct space by dividing the x coord with the space thickness and adding 800
                        markingIndex = (pointZ / 0.1) + (ZDIMENSION / 2);
                        Zspaces[markingIndex].tot = Zspaces[markingIndex].tot + pointZ;
                        Zspaces[markingIndex].num++;

                        newNode = newNode->next;
                        if(newNode == NULL) {

                            //printf("\nSTOP");
                        }
                        //Yspaces[1277].tot = 1528;
                        //printf("\nMedia selezionata: %f", Xspaces[418].tot / Xspaces[418].num);
                    }

                    //printf("\nSTART WATERMARKING EXTRACTION PHASE");

                    /*
                                 *****EXTRACTION ON X*****
                    */

                    //printf("\nSTART EXTRACTION");
                    bitCounter = 0;
                    //I repeat the operation for all the values in the array of indexes of spaces to extract
                    counter = 0;
                    while ((counter < XNUMBER) && (detected != 1)) {

                        currentValue = Xspaces[Xindexes[counter]].tot;
                        value = Xspaces[Xindexes[counter]].num;

                        //I check if there are points in the space considered (else I proceed with no more computations)
                        if (value != 0) {

                            //TODO: check the calculus (see the warning)
                            //This is now the mean
                            currentValue = currentValue / value;
                            //printf("\nXindexes[counter]: %d          bits[bitCounter]: %d       counter = %d", Xindexes[counter], bits[bitCounter], counter);

                            //TODO: CHECK CONVERSION
                            currentMean = (Xindexes[counter]) - (XDIMENSION / 2);
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
                                    detected = 1;
                                    tamperingX = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region X [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nXindexes[counter] = %d", Xindexes[counter]);
                                }
                            } else {

                                //I check if the values for the watermark are correct
                                if (currentValue >= currentMean) {

                                    detected = 1;
                                    tamperingX = 1;
                                    //In this case they do not correspond, I print out the tampered region
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region X [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nXindexes[counter] = %d", Xindexes[counter]);
                                }
                            }
                            bitCounter++;
                        }
                        counter++;
                        /*if(primo == 0) {

                            detected = 0;
                        }*/
                    }
                    //printf("\ntotalDetectedX: %d", selectedCounter);


                    /*
                                 *****MARK ON Y*****
                    */

                    //I repeat the operation for all the values in the array of indexes of spaces to extract
                    counter = 0;
                    while ((counter < YNUMBER) && (detected != 1)) {

                        value = 0;
                        currentValue = 0;

                        currentValue = Yspaces[Yindexes[counter]].tot;
                        value = Yspaces[Yindexes[counter]].num;

                        //I check if there are points in the space considered (else I proceed with no more computations)
                        if (value != 0) {

                            //TODO: check the calculus (see the warning)
                            //This is now the mean
                            currentValue = currentValue / value;
                            //printf("\nXMean: %f", currentValue);

                            //TODO: CHECK CONVERSION
                            currentMean = (Yindexes[counter]) - (YDIMENSION / 2);
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
                                    detected = 1;
                                    tamperingY = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Y [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nYindexes[counter] = %d", Yindexes[counter]);*/
                                }
                            } else {

                                //I check if the values for the watermark are correct
                                if (currentValue >= currentMean) {

                                    //In this case they do not correspond, I print out the tampered region
                                    detected = 1;
                                    tamperingY = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Y [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nYindexes[counter] = %d", Yindexes[counter]);*/
                                }
                            }
                            bitCounter++;
                        }
                        counter++;
                        /*if(primo == 0) {

                            detected = 0;
                        }*/
                    }
                    //printf("\ntotalDetectedY: %d", selectedCounter);



                    /*
                                 *****MARK ON Z*****
                    */
                    //printf("\nZ extraction");
                    //I repeat the operation for all the values in the array of indexes of spaces to mark
                    counter = 0;
                    while ((counter < ZNUMBER) && (detected != 1)) {

                        value = 0;
                        currentValue = 0;

                        currentValue = Zspaces[Zindexes[counter]].tot;
                        value = Zspaces[Zindexes[counter]].num;

                        //printf("\nZCurrent value: %f \nZValue: %d", currentValue, value);
                        //I check if there are points in the space considered (else I proceed with no more computations)
                        if (value != 0) {

                            //TODO: check the calculus (see the warning)
                            //This is now the mean
                            currentValue = currentValue / value;
                            //printf("\nZMean: %f", currentValue);

                            //TODO: CHECK CONVERSION
                            currentMean = (Zindexes[counter]) - (ZDIMENSION / 2);
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
                                    detected = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Z [ %f , %f ]    CounterX: %f    CounterY: %f    CounterZ: %f", (currentMean - 0.05),
                                           (currentMean + 0.05), counterX, counterY, counterZ);
                                }
                            } else {

                                //I check if I have to do some changes in point positions due to the watermark insertion
                                if (currentValue >= currentMean) {

                                    //printf("\nZModifico");
                                    //In this case they do not correspond, I print out the tampered region
                                    detected = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Z [ %f , %f ]    CounterX: %f    CounterY: %f    CounterZ: %f", (currentMean - 0.05),
                                           (currentMean + 0.05), counterX, counterY, counterZ);
                                }
                            }
                            bitCounter++;
                        }
                        counter++;
                        /*if(primo == 0) {

                            detected = 0;
                        }*/
                    }
                    //printf("\ntotalDetectedZ: %d", selectedCounter);



                    if (detected == 1) {

                        success++;
                    } else {

                        printf("\n\nTAMPERING NOT DETECTED!   CounterX: %f    CounterY: %f    CounterZ: %f\n\n", counterX, counterY, counterZ);
                        unsuccess++;
                    }
                    total++;

                    printf("\ntotalDetected: %d", selectedCounter);

                    for(i = 0; i < ZDIMENSION; i++) {

                        Xspaces[i].tot = Xspaces[i].old_sum;
                        Xspaces[i].num = Xspaces[i].old_num;
                        Yspaces[i].tot = Yspaces[i].old_sum;
                        Yspaces[i].num = Yspaces[i].old_num;
                        Zspaces[i].tot = Zspaces[i].old_sum;
                        Zspaces[i].num = Zspaces[i].old_num;
                    }

                    //printf("\nOldSum: %f", Yspaces[61].old_sum / Yspaces[61].old_num);

                    while (i < XDIMENSION) {

                        Xspaces[i].tot = Xspaces[i].old_sum;
                        Xspaces[i].num = Xspaces[i].old_num;
                        Yspaces[i].tot = Yspaces[i].old_sum;
                        Yspaces[i].num = Yspaces[i].old_num;
                        Zspaces[i].tot = Zspaces[i].old_sum;
                        Zspaces[i].num = Zspaces[i].old_num;
                        i++;
                    }



                } else {

                    total++;
                    success++;
                }
                counterZ = counterZ + 1;
            }
            counterZ = 0;
            //printf("\nCounterY: %f", counterY);
            counterY = counterY + 1;
        }
        counterY = 0;
        counterX = counterX - 1;
        //printf("\nCounterX: %f", counterX);
    }

    counterX = 1;
    counterY = 0;
    counterZ = 0;

    while(counterX + x < 78) {

        tamperingX = 0;
        while(counterY + y < 78) {

            tamperingY = 0;
            while(counterZ + z < 30) {

                //printf("\nEntro nel whileZ");
                if((tamperingX == 0) && (tamperingY == 0)) {

                    //printf("\nEntro nell'ifZ");
                    detected = 0;

                    /*if(counterX + x - 1 < -78) {

                        primo = 0;
                    }*/

                    selectedCounter = 0;
                    newNode = headSelected;
                    //printf("\nSono prima del whileZ");
                    while(newNode != NULL) {

                        //printf("\nEntro nel secondo whileZ, %f", newNode->point.x);
                        pointX = newNode->point.x + counterX;
                        pointY = newNode->point.y + counterY;
                        pointZ = newNode->point.z + counterZ;
                        pointR = newNode->point.r;

                        markingIndex = (pointX / 0.1) + (XDIMENSION / 2);

                        Xspaces[markingIndex].tot = Xspaces[markingIndex].tot + pointX;
                        Xspaces[markingIndex].num++;

                        markingIndex = (pointY / 0.1) + (YDIMENSION / 2);

                        Yspaces[markingIndex].tot = Yspaces[markingIndex].tot + pointY;
                        Yspaces[markingIndex].num++;

                        //printf("Y: %f ", currentValue);
                        //I select the correct space by dividing the x coord with the space thickness and adding 800
                        markingIndex = (pointZ / 0.1) + (ZDIMENSION / 2);
                        Zspaces[markingIndex].tot = Zspaces[markingIndex].tot + pointZ;
                        Zspaces[markingIndex].num++;

                        newNode = newNode->next;
                        if(newNode == NULL) {

                            //printf("\nSTOP");
                        }
                        //Yspaces[1277].tot = 1528;
                        //printf("\nMedia selezionata: %f", Xspaces[418].tot / Xspaces[418].num);
                    }

                    //printf("\nSTART WATERMARKING EXTRACTION PHASE");

                    /*
                                 *****EXTRACTION ON X*****
                    */

                    //printf("\nSTART EXTRACTION");
                    bitCounter = 0;
                    //I repeat the operation for all the values in the array of indexes of spaces to extract
                    counter = 0;
                    while ((counter < XNUMBER) && (detected != 1)) {

                        currentValue = Xspaces[Xindexes[counter]].tot;
                        value = Xspaces[Xindexes[counter]].num;

                        //I check if there are points in the space considered (else I proceed with no more computations)
                        if (value != 0) {

                            //TODO: check the calculus (see the warning)
                            //This is now the mean
                            currentValue = currentValue / value;
                            //printf("\nXindexes[counter]: %d          bits[bitCounter]: %d       counter = %d", Xindexes[counter], bits[bitCounter], counter);

                            //TODO: CHECK CONVERSION
                            currentMean = (Xindexes[counter]) - (XDIMENSION / 2);
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
                                    detected = 1;
                                    tamperingX = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region X [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nXindexes[counter] = %d", Xindexes[counter]);
                                }
                            } else {

                                //I check if the values for the watermark are correct
                                if (currentValue >= currentMean) {

                                    detected = 1;
                                    tamperingX = 1;
                                    //In this case they do not correspond, I print out the tampered region
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region X [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nXindexes[counter] = %d", Xindexes[counter]);
                                }
                            }
                            bitCounter++;
                        }
                        counter++;
                        /*if(primo == 0) {

                            detected = 0;
                        }*/
                    }
                    //printf("\ntotalDetectedX: %d", selectedCounter);


                    /*
                                 *****MARK ON Y*****
                    */

                    //I repeat the operation for all the values in the array of indexes of spaces to extract
                    counter = 0;
                    while ((counter < YNUMBER) && (detected != 1)) {

                        value = 0;
                        currentValue = 0;

                        currentValue = Yspaces[Yindexes[counter]].tot;
                        value = Yspaces[Yindexes[counter]].num;

                        //I check if there are points in the space considered (else I proceed with no more computations)
                        if (value != 0) {

                            //TODO: check the calculus (see the warning)
                            //This is now the mean
                            currentValue = currentValue / value;
                            //printf("\nXMean: %f", currentValue);

                            //TODO: CHECK CONVERSION
                            currentMean = (Yindexes[counter]) - (YDIMENSION / 2);
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
                                    detected = 1;
                                    tamperingY = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Y [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nYindexes[counter] = %d", Yindexes[counter]);*/
                                }
                            } else {

                                //I check if the values for the watermark are correct
                                if (currentValue >= currentMean) {

                                    //In this case they do not correspond, I print out the tampered region
                                    detected = 1;
                                    tamperingY = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Y [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nYindexes[counter] = %d", Yindexes[counter]);*/
                                }
                            }
                            bitCounter++;
                        }
                        counter++;
                        /*if(primo == 0) {

                            detected = 0;
                        }*/
                    }
                    //printf("\ntotalDetectedY: %d", selectedCounter);



                    /*
                                 *****MARK ON Z*****
                    */
                    //printf("\nZ extraction");
                    //I repeat the operation for all the values in the array of indexes of spaces to mark
                    counter = 0;
                    while ((counter < ZNUMBER) && (detected != 1)) {

                        value = 0;
                        currentValue = 0;

                        currentValue = Zspaces[Zindexes[counter]].tot;
                        value = Zspaces[Zindexes[counter]].num;

                        //printf("\nZCurrent value: %f \nZValue: %d", currentValue, value);
                        //I check if there are points in the space considered (else I proceed with no more computations)
                        if (value != 0) {

                            //TODO: check the calculus (see the warning)
                            //This is now the mean
                            currentValue = currentValue / value;
                            //printf("\nZMean: %f", currentValue);

                            //TODO: CHECK CONVERSION
                            currentMean = (Zindexes[counter]) - (ZDIMENSION / 2);
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
                                    detected = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Z [ %f , %f ]    CounterX: %f    CounterY: %f    CounterZ: %f", (currentMean - 0.05),
                                           (currentMean + 0.05), counterX, counterY, counterZ);
                                }
                            } else {

                                //I check if I have to do some changes in point positions due to the watermark insertion
                                if (currentValue >= currentMean) {

                                    //printf("\nZModifico");
                                    //In this case they do not correspond, I print out the tampered region
                                    detected = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Z [ %f , %f ]    CounterX: %f    CounterY: %f    CounterZ: %f", (currentMean - 0.05),
                                           (currentMean + 0.05), counterX, counterY, counterZ);
                                }
                            }
                            bitCounter++;
                        }
                        counter++;
                        /*if(primo == 0) {

                            detected = 0;
                        }*/
                    }
                    //printf("\ntotalDetectedZ: %d", selectedCounter);


                    //primo = 1;

                    if (detected == 1) {

                        success++;
                    } else {

                        printf("\n\nTAMPERING NOT DETECTED!   CounterX: %f    CounterY: %f    CounterZ: %f\n\n", counterX, counterY, counterZ);
                        unsuccess++;
                    }
                    total++;

                    printf("\ntotalDetected: %d", selectedCounter);

                    for(i = 0; i < ZDIMENSION; i++) {

                        Xspaces[i].tot = Xspaces[i].old_sum;
                        Xspaces[i].num = Xspaces[i].old_num;
                        Yspaces[i].tot = Yspaces[i].old_sum;
                        Yspaces[i].num = Yspaces[i].old_num;
                        Zspaces[i].tot = Zspaces[i].old_sum;
                        Zspaces[i].num = Zspaces[i].old_num;
                    }

                    //printf("\nOldSum: %f", Yspaces[61].old_sum / Yspaces[61].old_num);

                    while (i < XDIMENSION) {

                        Xspaces[i].tot = Xspaces[i].old_sum;
                        Xspaces[i].num = Xspaces[i].old_num;
                        Yspaces[i].tot = Yspaces[i].old_sum;
                        Yspaces[i].num = Yspaces[i].old_num;
                        Zspaces[i].tot = Zspaces[i].old_sum;
                        Zspaces[i].num = Zspaces[i].old_num;
                        i++;
                    }



                } else {

                    total++;
                    success++;
                }
                counterZ = counterZ + 1;
            }
            counterZ = 0;
            //printf("\nCounterY: %f", counterY);
            counterY = counterY + 1;
        }
        counterY = 0;
        counterX = counterX + 1;
        //printf("\nCounterX: %f", counterX);
    }

    counterX = 1;
    counterY = -1;
    counterZ = 0;

    while(counterX + x < 78) {

        tamperingX = 0;
        while(counterY + y > -78) {

            tamperingY = 0;
            while(counterZ + z < 30) {

                //printf("\nEntro nel whileZ");
                if((tamperingX == 0) && (tamperingY == 0)) {

                    //printf("\nEntro nell'ifZ");
                    detected = 0;

                    /*if(counterX + x - 1 < -78) {

                        primo = 0;
                    }*/

                    selectedCounter = 0;
                    newNode = headSelected;
                    //printf("\nSono prima del whileZ");
                    while(newNode != NULL) {

                        //printf("\nEntro nel secondo whileZ, %f", newNode->point.x);
                        pointX = newNode->point.x + counterX;
                        pointY = newNode->point.y + counterY;
                        pointZ = newNode->point.z + counterZ;
                        pointR = newNode->point.r;

                        markingIndex = (pointX / 0.1) + (XDIMENSION / 2);

                        Xspaces[markingIndex].tot = Xspaces[markingIndex].tot + pointX;
                        Xspaces[markingIndex].num++;

                        markingIndex = (pointY / 0.1) + (YDIMENSION / 2);

                        Yspaces[markingIndex].tot = Yspaces[markingIndex].tot + pointY;
                        Yspaces[markingIndex].num++;

                        //printf("Y: %f ", currentValue);
                        //I select the correct space by dividing the x coord with the space thickness and adding 800
                        markingIndex = (pointZ / 0.1) + (ZDIMENSION / 2);
                        Zspaces[markingIndex].tot = Zspaces[markingIndex].tot + pointZ;
                        Zspaces[markingIndex].num++;

                        newNode = newNode->next;
                        if(newNode == NULL) {

                            //printf("\nSTOP");
                        }
                        //Yspaces[1277].tot = 1528;
                        //printf("\nMedia selezionata: %f", Xspaces[418].tot / Xspaces[418].num);
                    }

                    //printf("\nSTART WATERMARKING EXTRACTION PHASE");

                    /*
                                 *****EXTRACTION ON X*****
                    */

                    //printf("\nSTART EXTRACTION");
                    bitCounter = 0;
                    //I repeat the operation for all the values in the array of indexes of spaces to extract
                    counter = 0;
                    while ((counter < XNUMBER) && (detected != 1)) {

                        currentValue = Xspaces[Xindexes[counter]].tot;
                        value = Xspaces[Xindexes[counter]].num;

                        //I check if there are points in the space considered (else I proceed with no more computations)
                        if (value != 0) {

                            //TODO: check the calculus (see the warning)
                            //This is now the mean
                            currentValue = currentValue / value;
                            //printf("\nXindexes[counter]: %d          bits[bitCounter]: %d       counter = %d", Xindexes[counter], bits[bitCounter], counter);

                            //TODO: CHECK CONVERSION
                            currentMean = (Xindexes[counter]) - (XDIMENSION / 2);
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
                                    detected = 1;
                                    tamperingX = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region X [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nXindexes[counter] = %d", Xindexes[counter]);
                                }
                            } else {

                                //I check if the values for the watermark are correct
                                if (currentValue >= currentMean) {

                                    detected = 1;
                                    tamperingX = 1;
                                    //In this case they do not correspond, I print out the tampered region
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region X [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nXindexes[counter] = %d", Xindexes[counter]);
                                }
                            }
                            bitCounter++;
                        }
                        counter++;
                        /*if(primo == 0) {

                            detected = 0;
                        }*/
                    }
                    //printf("\ntotalDetectedX: %d", selectedCounter);


                    /*
                                 *****MARK ON Y*****
                    */

                    //I repeat the operation for all the values in the array of indexes of spaces to extract
                    counter = 0;
                    while ((counter < YNUMBER) && (detected != 1)) {

                        value = 0;
                        currentValue = 0;

                        currentValue = Yspaces[Yindexes[counter]].tot;
                        value = Yspaces[Yindexes[counter]].num;

                        //I check if there are points in the space considered (else I proceed with no more computations)
                        if (value != 0) {

                            //TODO: check the calculus (see the warning)
                            //This is now the mean
                            currentValue = currentValue / value;
                            //printf("\nXMean: %f", currentValue);

                            //TODO: CHECK CONVERSION
                            currentMean = (Yindexes[counter]) - (YDIMENSION / 2);
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
                                    detected = 1;
                                    tamperingY = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Y [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nYindexes[counter] = %d", Yindexes[counter]);*/
                                }
                            } else {

                                //I check if the values for the watermark are correct
                                if (currentValue >= currentMean) {

                                    //In this case they do not correspond, I print out the tampered region
                                    detected = 1;
                                    tamperingY = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Y [ %f , %f ]   CurrentValue=%f   valore letto: %d    CounterX: %f    CounterY: %f    CounterZ: %f",
                                           (currentMean - 0.05), (currentMean + 0.05), currentValue,
                                           bits[bitCounter], counterX, counterY, counterZ);
                                    //printf("\nYindexes[counter] = %d", Yindexes[counter]);*/
                                }
                            }
                            bitCounter++;
                        }
                        counter++;
                        /*if(primo == 0) {

                            detected = 0;
                        }*/
                    }
                    //printf("\ntotalDetectedY: %d", selectedCounter);



                    /*
                                 *****MARK ON Z*****
                    */
                    //printf("\nZ extraction");
                    //I repeat the operation for all the values in the array of indexes of spaces to mark
                    counter = 0;
                    while ((counter < ZNUMBER) && (detected != 1)) {

                        value = 0;
                        currentValue = 0;

                        currentValue = Zspaces[Zindexes[counter]].tot;
                        value = Zspaces[Zindexes[counter]].num;

                        //printf("\nZCurrent value: %f \nZValue: %d", currentValue, value);
                        //I check if there are points in the space considered (else I proceed with no more computations)
                        if (value != 0) {

                            //TODO: check the calculus (see the warning)
                            //This is now the mean
                            currentValue = currentValue / value;
                            //printf("\nZMean: %f", currentValue);

                            //TODO: CHECK CONVERSION
                            currentMean = (Zindexes[counter]) - (ZDIMENSION / 2);
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
                                    detected = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Z [ %f , %f ]    CounterX: %f    CounterY: %f    CounterZ: %f", (currentMean - 0.05),
                                           (currentMean + 0.05), counterX, counterY, counterZ);
                                }
                            } else {

                                //I check if I have to do some changes in point positions due to the watermark insertion
                                if (currentValue >= currentMean) {

                                    //printf("\nZModifico");
                                    //In this case they do not correspond, I print out the tampered region
                                    detected = 1;
                                    selectedCounter++;
                                    printf("\nTAMPERING DETECTED!   Region Z [ %f , %f ]    CounterX: %f    CounterY: %f    CounterZ: %f", (currentMean - 0.05),
                                           (currentMean + 0.05), counterX, counterY, counterZ);
                                }
                            }
                            bitCounter++;
                        }
                        counter++;
                        /*if(primo == 0) {

                            detected = 0;
                        }*/
                    }
                    //printf("\ntotalDetectedZ: %d", selectedCounter);


                    //primo = 1;

                    if (detected == 1) {

                        success++;
                    } else {

                        printf("\n\nTAMPERING NOT DETECTED!   CounterX: %f    CounterY: %f    CounterZ: %f\n\n", counterX, counterY, counterZ);
                        unsuccess++;
                    }
                    total++;

                    printf("\ntotalDetected: %d", selectedCounter);

                    for(i = 0; i < ZDIMENSION; i++) {

                        Xspaces[i].tot = Xspaces[i].old_sum;
                        Xspaces[i].num = Xspaces[i].old_num;
                        Yspaces[i].tot = Yspaces[i].old_sum;
                        Yspaces[i].num = Yspaces[i].old_num;
                        Zspaces[i].tot = Zspaces[i].old_sum;
                        Zspaces[i].num = Zspaces[i].old_num;
                    }

                    //printf("\nOldSum: %f", Yspaces[61].old_sum / Yspaces[61].old_num);

                    while (i < XDIMENSION) {

                        Xspaces[i].tot = Xspaces[i].old_sum;
                        Xspaces[i].num = Xspaces[i].old_num;
                        Yspaces[i].tot = Yspaces[i].old_sum;
                        Yspaces[i].num = Yspaces[i].old_num;
                        Zspaces[i].tot = Zspaces[i].old_sum;
                        Zspaces[i].num = Zspaces[i].old_num;
                        i++;
                    }



                } else {

                    total++;
                    success++;
                }
                counterZ = counterZ + 1;
            }
            counterZ = 0;
            //printf("\nCounterY: %f", counterY);
            counterY = counterY - 1;
        }
        counterY = 0;
        counterX = counterX + 1;
        //printf("\nCounterX: %f", counterX);
    }

    success = success+2;
    printf("\n\nTOTALI: %d \nDETECTED: %d \nNOT DETECTED: %d\n\n", total, success, unsuccess);

}













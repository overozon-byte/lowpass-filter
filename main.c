#include <stdio.h>
#include <stdlib.h>
#define NCoef 2


//Fs = 44100
//Fc = 3000 Hz
//To 5000 Hz
double iir(double NewSample) {
    double ACoef[NCoef+1] = {
        0.03478271538033044600,
        0.06956543076066089300,
        0.03478271538033044600
    };

    double BCoef[NCoef+1] = {
        1.00000000000000000000,
        -1.40750534391514300000,
        0.54664949368209625000
    };

    static double y[NCoef+1]; //output samples
    static double x[NCoef+1]; //input samples
    int n;

    //shift the old samples
    for(n=NCoef; n>0; n--) {
       x[n] = x[n-1];
       y[n] = y[n-1];
    }

    //Calculate the new output
    x[0] = NewSample;
    y[0] = ACoef[0] * x[0];
    for(n=1; n<=NCoef; n++)
        y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];

    return y[0];
}


int main(void)
{
    FILE* fp_r; // for read
    FILE* fp_w; // for write
    int size = 0;
    char need_bytes[44];


    if ((fp_r = fopen("E:\\Projects\\Polytech\\3\\filter\\lowpass\\audio\\mono.wav", "r+b")) == NULL) {
        printf("Cannot open file for read.\n");
        return (1);
    }

    if ((fp_w = fopen("E:\\Projects\\Polytech\\3\\filter\\lowpass\\audio\\mono_vs.wav", "w+b")) == NULL) {
        printf("Cannot open file for write.\n");
        return (1);
    }

    fseek(fp_r, 0L, SEEK_END); //To the end of file
    size = ftell(fp_r); //Take size 
    fseek(fp_r, 0L, SEEK_SET); //To the begin of file

    //Read 44 bytes from mono(.wav)
    fread(need_bytes, sizeof(need_bytes[0]), 44, fp_r);
    fwrite(need_bytes, sizeof(need_bytes[0]), 44, fp_w);
    printf("%s", need_bytes,"\n");
    
    //Input buffer for dynamic memory
    short* inputbuffer = (short*)malloc(size * sizeof(short));
    if (inputbuffer == NULL) {
        printf("Memory allocation for inputbuffer failed!\n");
        return 1;
    }
    //Output buffer for dynamic memory
    short* outputbuffer = (short*)malloc(size * sizeof(short));
    if (outputbuffer == NULL) {
        printf("Memory allocation for outputbuffer failed!\n");
        return 1;
    }
    //Read (size of mono) - 44 bytes in dynamic memory 
    fread(inputbuffer, sizeof(inputbuffer[0]), size, fp_r);

    double* mi = (double*)malloc(size * sizeof(double));
    if (mi == NULL) {
        printf("Memory allocation for mi failed!\n");
        return 1;
    }
    
    //From short to double
    for (int i = 0; i < size; i++) {
        mi[i] = (double)inputbuffer[i];
        if (mi[i] >= 32767) { // per on sample 16 bit
            mi[i] = 32767;
        }
        else {
            if (mi[i] <= (-32768)) {
                mi[i] = (-32768);
            }
            else {
                mi[i] = mi[i];
            }
        }
    }

    //From double to short
    for (int i = 0; i < size; i++) {
        outputbuffer[i] = (short)(iir(mi[i]));
    }

    //Wirte in second file mono_vs(.wav)
    fwrite(outputbuffer, sizeof(outputbuffer[0]), size, fp_w);

    free(inputbuffer);
    free(outputbuffer);
    free(mi);
   
    fclose(fp_r);
    fclose(fp_w);


    return (0);
}

/**
 * 
 * Program to implement a linear phase FIR filter
 * 
 * Reference: Shawn Stevenson's blog @ https://sestevenson.wordpress.com/implementation-of-fir-filtering-in-c-part-1/
 * 
 * website: www.technogeek310.wordpress.com
 * author: Rishi K Shukla
 * 
 */ 

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_INPUT_LEN 100                                   // max. input that can be handled in one function call
#define MAX_FLT_LEN   63                                    // max. filter length that can be handled
#define BUFFER_LEN  (MAX_FLT_LEN - 1 + MAX_INPUT_LEN)       // buffer to hold input samples    
#define SAMPLES 100                                         // number of samples to read per loop                    

double input_samples[BUFFER_LEN];                           // array to hold input samples

/**
 * bandpass filter coefficients with filter centered around 5000 Hz
 * sampling rate = 44100 Hz
 */
double coeffs[MAX_FLT_LEN] = {
    0.0730925,   0.100557,   -0.1322851,   0.0954512,   0.0754908,
    0.0915603,   0.0366887,  -0.0113547,  -0.051204,   -0.0602071,
   -0.0419634,  -0.0059188,   0.0298679,   0.0497159,   0.0453203,
    0.0197406,  -0.0152828,  -0.0427857,  -0.0494711,  -0.0323446,
    0.0008235,   0.034008,    0.0511223,   0.0433663,   0.0142687,
   -0.0223644,  -0.0484856,  -0.0508951,  -0.0287151,   0.007856,
    0.0407296,   0.0536207,   0.0407296,   0.007856,   -0.0287151,
   -0.0508951,  -0.0484856,  -0.0223644,   0.0142687,   0.0433663,
    0.0511223,   0.034008,    0.0008235,  -0.0323446,  -0.0494711,
   -0.0427857,  -0.0152828,   0.0197406,   0.0453203,   0.0497159,
    0.0298679,  -0.0059188,  -0.0419634,  -0.0602071,  -0.051204,
   -0.0113547,   0.0366887,   0.0915603,   0.0754908,   0.0954512,
   -0.1322851,   0.100557,    0.0730925
};

/**
 * @brief function to initialize FIR filter
 */
void fir_init(void)
{
    memset(input_samples, 0, sizeof(input_samples));
}

/**
 * @brief function to implement a FIR filter
 */
void fir_func(double *coeffs, double *input, double *output, int length, int filter_length)
{
    double acc;             // accumulator for MACs
    double *pCoeff;         // pointer to coefficients
    double *pInput;         // pointer to input samples
    int i, j;
    
    // append new samples at the high end of the buffer
    memcpy(&input_samples[filter_length - 1], input, length * sizeof(double));

    // apply filter to each input sample
    for(i = 0; i < length; i++) {
        // calculate output i
        pCoeff = coeffs;
        pInput = &input_samples[filter_length - 1 + i];
        acc = 0;
        
        for (j = 0; j < filter_length; j++) {
            acc += (*pCoeff++) * (*pInput--);
        }
        output[i] = acc;
    }
    // shift input samples back in time for next data set
    memmove(&input_samples[0], &input_samples[length], (filter_length - 1) * sizeof(double));
} 

/**
 * @brief function to convert int to float 
 */
void int_2_float(int16_t *input, double *output, int length)
{
    int i;

    for(i = 0; i < length; i++) {
        // convert
        output[i] = (double)input[i];
    }
}

/**
 * @brief function to convert float to int
 */
void float_2_int(double *input, int16_t *output, int length)
{
    int i;

    for(i = 0; i < length; i++) {
        if(input[i] > 32767.0) {
            input[i] = 32767.0;
        }
        else if(input[i] < -32768.0) {
            input[i] = -32768.0;
        }
        // convert
        output[i] = (int16_t)input[i];
    }
}

/**
 * @brief Entry to main()
 */
void main(void)
{
    int size;
    int16_t input[SAMPLES];
    int16_t output[SAMPLES];
    double floatInput[SAMPLES];
    double floatOutput[SAMPLES];

    FILE   *in_fid;
    FILE   *out_fid;
 
    // open the input waveform file
    in_fid = fopen("dtmf.wav", "rb");
    if ( in_fid == 0 ) {
        printf("couldn't open dtmf.wav");
        return;
    }
 
    // open the output waveform file
    out_fid = fopen("dtmf-output.wav", "wb");
    if ( out_fid == 0 ) {
        printf("couldn't open dtmf-output.wav");
        return;
    }
 
    // initialize the filter
    fir_init();
 
    // process all of the samples
    do {
        // read samples from file
        size = fread(input, sizeof(int16_t), SAMPLES, in_fid);
        // convert to doubles
        int_2_float(input, floatInput, size);
        // perform the filtering
        fir_func(coeffs, floatInput, floatOutput, size, MAX_FLT_LEN);
        // convert to ints
        float_2_int(floatOutput, output, size);
        // write samples to file
        fwrite(output, sizeof(int16_t), size, out_fid);
    } while (size != 0);
 
    fclose(in_fid);
    fclose(out_fid);
}
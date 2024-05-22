#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#define K_0 37
#define K_1 109
#define K_2 109
#define K_3 37

#define BURST_LEN 4 // minimum of 4
#define SAMPLES 16340

int filter(int x0, int x1, int x2, int x3);
int sample_read();
void sample_write(int dval);
void sample_copy(int length);
int16_t mulf(int16_t i1, int16_t i2);
int16_t addf(int16_t i1, int16_t i2);

int main() {
    
    // delay line and input to 0
    //int output = 0;
    int input[BURST_LEN] = {0};
	int temp_in[BURST_LEN] = {0};
	int temp_out[BURST_LEN] = {0};
	int status = 1;

    // copy wav file header
    sample_copy(44);

    for (int i = 0; i < SAMPLES; i += BURST_LEN)
    {
		// fill input buffer
		for (int j = 0; j < BURST_LEN; j++)
		{
			temp_in[j] = sample_read();
		}
		
		// move data from buffer to processing and filter into output buffer
		for (int k = 0; k < BURST_LEN; k++)
		{
			for (int a = BURST_LEN - 1; a > 0; a--)
			{
				input[a] = input[a-1];
			}

			input[0] = temp_in[0];

			for (int b = 0; b < BURST_LEN - 1; b++)
			{
				temp_in[b] = temp_in[b+1];
			}

			temp_in[BURST_LEN-1] = 0;

       		temp_out[k] = filter(input[0], input[1], input[2], input[3]);
		}
        
		// empty output buffer
		for (int l = 0; l < BURST_LEN; l++)
		{
			sample_write(temp_out[l]);
			temp_out[l] = 0;
		}
    }
    return 0;
}

int filter(int x0, int x1, int x2, int x3)
{
    //return (x0*K_0 + x1*K_1 + x2*K_2 + x3*K_3);
	return (int)addf(mulf(x0,K_0),addf(mulf(x1,K_1),addf(mulf(x2,K_2),mulf(x3,K_3))));
}

int sample_read() {
	int val;
	int inval;
	int status = 1;
	_TCEFU_FIFO_S16_STREAM_IN("INPUT", 0, val, status);
	inval = (int)(val - 128); // convert to int
	return inval;
}
	
void sample_write(int dval) {
	int outval;
	outval = (int)(dval + 128); // convert to uint
	_TCEFU_FIFO_S16_STREAM_OUT("OUTPUT", outval);
}

void sample_copy(int length) {
	int i;
	int status = 1;
	for (i = 0; i < length; i ++ ) {
		int val;
		_TCEFU_FIFO_S16_STREAM_IN("INPUT", 0, val, status);
		_TCEFU_FIFO_S16_STREAM_OUT("OUTPUT", val);
	}
}

int16_t mulf(int16_t i1, int16_t i2)
{
	int32_t intermediate = (int32_t)((int16_t)i1 * (int16_t)i2);
	return (int16_t)(intermediate >> 15);
}

int16_t addf(int16_t i1, int16_t i2)
{
	int32_t intermediate = (int32_t)((int16_t)i1 + (int16_t)i2);
	return (int16_t)intermediate;
}

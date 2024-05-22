#include <inttypes.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#define BURST_LEN 4
#define SAMPLES 5626

int16_t sample_read();
int16_t sample_read_delay();
void sample_write(int16_t val);
void sample_copy(int length);
void sample_discard(int length);
int16_t mulf(int16_t i1, int16_t i2);
int16_t addf(int16_t i1, int16_t i2);
int16_t subf(int16_t i1, int16_t i2);
int16_t filter(int16_t x0, int16_t x1, int16_t d);

int main ()  {
	int i;

	int16_t x0 = 0, x1 = 0, d = 0;
	int16_t buff_in[BURST_LEN] = {0};
	int16_t buff_del[BURST_LEN] = {0};
	int16_t buff_out[BURST_LEN] = {0};
	//int16_t output = 0;

	sample_copy(44);
	sample_discard(44);
	
	for (i = 0; i < SAMPLES; i += BURST_LEN) {
		
		// fill the input buffer
		for (int j = 0; j < BURST_LEN; j++)
		{
			buff_in[j] = sample_read();
			buff_del[j] = sample_read_delay();
		}

		// filter
		for (int k = 0; k < BURST_LEN; k++)
		{
			x1 = x0;
			x0 = buff_in[0];
			for (int a = 0; a < BURST_LEN - 1; a++)
			{
				buff_in[a] = buff_in[a+1];
			}
			d = buff_del[0];
			for (int b = 0; b < BURST_LEN - 1; b++)
			{
				buff_del[b] = buff_del[b+1];
			}
			buff_out[k] = filter(x0,x1,d);
		}

		// empty buffer
		for (int l = 0; l < BURST_LEN; l++)
		{
			sample_write(buff_out[l]);
		}
	}

	return 0;
}

int16_t filter(int16_t x0, int16_t x1, int16_t d) {
	// y(n) = [1-d(n)]*x(n) + d(n)*x(n-1)
	return addf(mulf(subf(0x40, d), x0), mulf(d, x1));
}

int16_t sample_read() {
	int16_t val;
	int inval;
	uint8_t status;
	_TCEFU_FIFO_S16_STREAM_IN("INPUT_2", 0, inval, status);
	val = (int16_t)(inval << 6);
	return val;
}

int16_t sample_read_delay() {
	int16_t val;
	uint8_t inval;
	uint8_t status;
	_TCEFU_FIFO_S16_STREAM_IN("INPUT_1", 0, inval, status);
	val = (int16_t)(inval << 2);
	return val;
}
	
void sample_write(int16_t val) {
	uint8_t outval;
	outval = (uint8_t)(val >> 6);
	_TCEFU_FIFO_S16_STREAM_OUT("OUTPUT", outval);
}

void sample_copy(int length) {
	int i;
	uint8_t status;
	for (i = 0; i < length; i++ ) {
		uint8_t val;
		_TCEFU_FIFO_S16_STREAM_IN("INPUT_2", 0, val, status);
		_TCEFU_FIFO_S16_STREAM_OUT("OUTPUT", val);
	}
}

void sample_discard(int length) {
	int i;
	uint8_t status;
	for (i = 0; i < length; i++ ) {
		uint8_t val;
		_TCEFU_FIFO_S16_STREAM_IN("INPUT_1", 0, val, status);
	}
}

int16_t mulf(int16_t i1, int16_t i2)
{
	int32_t intermediate = (int32_t)((int16_t)i1 * (int16_t)i2);
	return (int16_t)(intermediate >> 6);
}

int16_t addf(int16_t i1, int16_t i2)
{
	int32_t intermediate = (int32_t)((int16_t)i1 + (int16_t)i2);
	return (int16_t)intermediate;
}

int16_t subf(int16_t i1, int16_t i2)
{
	int32_t intermediate = (int32_t)((int16_t)i1 - (int16_t)i2);
	return (int16_t)intermediate;
}

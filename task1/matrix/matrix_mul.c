#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define FIXED_POINT_FRACTIONAL_BITS 5

typedef uint16_t HALF;
typedef uint16_t fixed_point_t;


void matrix_mul(float * A, float * B, float *Result, int rows_of_A, int columns_of_A ,int rows_of_B, int columns_of_B, bool Fix_Or_Float16);
float HALFToFloat(HALF);
HALF floatToHALF(float);
static uint32_t halfToFloatI(HALF);
static HALF floatToHalfI(uint32_t);
/// Converts 11.5 format -> double
double fixed_to_double(fixed_point_t input);
/// Converts double to 11.5 format
fixed_point_t double_to_fixed(double input);

int main(void)
{
    // matrices A and B need to be formatted into a 1 x (rows x columns) vector format
    // 2x3 matrix
    float test_mat1[6] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6};
    int size_1[2] = {2, 3};
    // 3x2 matrix
    float test_mat2[6] = {6.6, 5.5, 4.4, 3.3, 2.2, 1.1};
    int size_2[2] = {3, 2};
    // 2x2 matrix
    float Result[4] = {0.0};
    
    matrix_mul(test_mat1, test_mat2, Result, size_1[0], size_1[1], size_2[0], size_2[1], true);
 
    return 0;
}

void matrix_mul(float A[], float B[], float Result[], int rows_of_A, int columns_of_A ,int rows_of_B, int columns_of_B, bool Fix_Or_Float16)
{
    
    float A_mat[100][100] = {0};
    float B_mat[100][100] = {0};
    
    printf("A matrix:\n");
    for (int i = 0; i < rows_of_A; i++)
    {
        for (int j = 0; j < columns_of_A; j++)
        {
            A_mat[i][j] = A[i*columns_of_A + j];
            printf("%f ", A_mat[i][j]);
        }
        printf("\n");
    }
    
    printf("B matrix:\n");
    for (int i = 0; i < rows_of_B; i++)
    {
        for (int j = 0; j < columns_of_B; j++)
        {
            B_mat[i][j] = B[i*columns_of_B + j];
            printf("%f ", B_mat[i][j]);
        }
        printf("\n");
    }
    
    if (columns_of_A != rows_of_B)
    {
        printf("Matrix dimension mismatch!\n");
        return;
    }
    
    float result[100][100] = {0};
        
    if (Fix_Or_Float16)
    {
        // if true, fixed-point
        for (int i = 0; i < rows_of_A; ++i)
        {
            for (int j = 0; j < columns_of_B; ++j)
            {
                // result[i][j] = 0;
                for (int k = 0; k < columns_of_A; ++k)
                {
                    result[i][j] += fixed_to_double(double_to_fixed(A_mat[i][k] * B_mat[k][j]));
                }
            }
        }
    }
    else
    {
        // if not true, float16
        for (int i = 0; i < rows_of_A; ++i)
        {
            for (int j = 0; j < columns_of_B; ++j)
            {
                // result[i][j] = 0;
                for (int k = 0; k < columns_of_A; ++k)
                {
                    result[i][j] += HALFToFloat(floatToHALF(A_mat[i][k] * B_mat[k][j]));
                }
            }
        }
    }
    
    
    // print result matrix
    printf("Result matrix:\n");
    for (int i = 0; i < rows_of_A; ++i) {
        for (int j = 0; j < columns_of_B; ++j) {
            printf("%f ", result[i][j]);
        }
        printf("\n");
    }
}

float
HALFToFloat(HALF y)
{
    union { float f; uint32_t i; } v;
    v.i = halfToFloatI(y);
    return v.f;
}

uint32_t
static halfToFloatI(HALF y)
{
    int s = (y >> 15) & 0x00000001;                            // sign
    int e = (y >> 10) & 0x0000001f;                            // exponent
    int f =  y        & 0x000003ff;                            // fraction

    // need to handle 7c00 INF and fc00 -INF?
    if (e == 0) {
        // need to handle +-0 case f==0 or f=0x8000?
        if (f == 0)                                            // Plus or minus zero
            return s << 31;
        else {                                                 // Denormalized number -- renormalize it
            while (!(f & 0x00000400)) {
                f <<= 1;
                e -=  1;
            }
            e += 1;
            f &= ~0x00000400;
        }
    } else if (e == 31) {
        if (f == 0)                                             // Inf
            return (s << 31) | 0x7f800000;
        else                                                    // NaN
            return (s << 31) | 0x7f800000 | (f << 13);
    }

    e = e + (127 - 15);
    f = f << 13;

    return ((s << 31) | (e << 23) | f);
}

HALF
floatToHALF(float i)
{
    union { float f; uint32_t i; } v;
    v.f = i;
    return floatToHalfI(v.i);
}

HALF
static floatToHalfI(uint32_t i)
{
    register int s =  (i >> 16) & 0x00008000;                   // sign
    register int e = ((i >> 23) & 0x000000ff) - (127 - 15);     // exponent
    register int f =   i        & 0x007fffff;                   // fraction

    // need to handle NaNs and Inf?
    if (e <= 0) {
        if (e < -10) {
            if (s)                                              // handle -0.0
               return 0x8000;
            else
               return 0;
        }
        f = (f | 0x00800000) >> (1 - e);
        return s | (f >> 13);
    } else if (e == 0xff - (127 - 15)) {
        if (f == 0)                                             // Inf
            return s | 0x7c00;
        else {                                                  // NAN
            f >>= 13;
            return s | 0x7c00 | f | (f == 0);
        }
    } else {
        if (e > 30)                                             // Overflow
            return s | 0x7c00;
        return s | (e << 10) | (f >> 13);
    }
}

inline double fixed_to_double(fixed_point_t input)
{
    return ((double)input / (double)(1 << FIXED_POINT_FRACTIONAL_BITS));
}

inline fixed_point_t double_to_fixed(double input)
{
    return (fixed_point_t)(input * (1 << FIXED_POINT_FRACTIONAL_BITS));
}

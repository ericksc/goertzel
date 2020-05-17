#include <stdio.h>
#include <math.h>
#define SAMPLE unsigned char
#define SAMPLING_RATE 8000.0 //8kHz
#define TARGET_FREQUENCY 941.0 //941 Hz
#define N 205 //Block size
#define PI 3.14159265358979
double coeff;
double Q1;
double Q2;
double sine;
double cosine;
SAMPLE testData[N];

double coeff_max;
double Q0_max;
double Q1_max;
double Q2_max;

void StoreMaxValues(double Q0)
{
    if (fabs(coeff)  > fabs(coeff_max))
    {
        coeff_max = coeff;
    }
    if (fabs(Q0)  > fabs(Q0_max))
    {
        Q0_max = Q0;
    }
    if (fabs(Q1)  > fabs(Q1_max))
    {
        Q1_max = Q1;
    }
    if (fabs(Q2)  > fabs(Q2_max))
    {
        Q2_max = Q2;
    }
}

void PrintStoreMaxValues(void)
{
    printf("For coeff_max = %f,\n", coeff_max);
    printf("For Q0_max = %f,\n", Q0_max);
    printf("For Q1_max = %f,\n", Q1_max);
    printf("For Q2_max = %f,\n", Q2_max);
}

/* Call this routine before every "block" (size=N) of samples. */
void ResetGoertzel(void)
{
    Q2 = 0;
    Q1 = 0;
}
/* Call this once, to precompute the constants. */
void InitGoertzel(void)
{
    int k;
    double floatN;
    double omega;
    floatN = (double)N;
    k = (int)(0.5 + ((floatN * TARGET_FREQUENCY) / SAMPLING_RATE));
    omega = (2.0 * PI * k) / floatN;
    sine = sin(omega);
    cosine = cos(omega);
    coeff = 2.0 * cosine;
    printf("For SAMPLING_RATE = %f", SAMPLING_RATE);
    printf(" N = %d", N);
    printf(" and FREQUENCY = %f,\n", TARGET_FREQUENCY);
    printf("k = %d and coeff = %f\n\n", k, coeff);
    ResetGoertzel();
}
/* Call this routine for every sample. */
void ProcessSample(SAMPLE sample)
{
    double Q0;
    Q0 = coeff * Q1 - Q2 + (double)sample;
    Q2 = Q1;
    Q1 = Q0;

    StoreMaxValues(Q0);
}

/* Optimized Goertzel */
/* Call this after every block to get the RELATIVE magnitude squared. */
double GetMagnitudeSquared(void)
{
    double result;
    result = Q1 * Q1 + Q2 * Q2 - Q1 * Q2 * coeff;
    return result;
}
/*** End of Goertzel-specific code, the remainder is test code. */
/* Synthesize some test data at a given frequency. */
void Generate(double frequency)
{
    int index;
    double step;
    step = frequency * ((2.0 * PI) / SAMPLING_RATE);
/* Generate the test data */
    for (index = 0; index < N; index++)
    {
        testData[index] = (SAMPLE)(100.0 * sin(index * step) + 100.0);
    }
}
/* Demo 1 */
void GenerateAndTest(double frequency)
{
    int index;
    double magnitudeSquared;
    printf("For test frequency %f:\n", frequency);
    Generate(frequency);
/* Process the samples */
    for (index = 0; index < N; index++)
    {
        ProcessSample(testData[index]);
    }
/* Do the "optimized Goertzel" processing */
    magnitudeSquared = GetMagnitudeSquared();
    printf("Relative magnitude squared = %f\n", magnitudeSquared);
    ResetGoertzel();
}
int main(void)
{
    InitGoertzel();
/* Demo 1 */
    GenerateAndTest(TARGET_FREQUENCY - 250);
    GenerateAndTest(TARGET_FREQUENCY);
    GenerateAndTest(TARGET_FREQUENCY + 250);
    PrintStoreMaxValues();
    return 0;
}
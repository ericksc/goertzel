#include <stdio.h>
#include <math.h>
#define SAMPLE unsigned char
#define SAMPLING_RATE 8000.0 //8kHz
#define TARGET_FREQUENCY 941.0 //941 Hz
#define N 205 //Block size
#define PI 3.14159265358979
float coeff;
int fix_coeff;
int Q1;
int Q2;
double sine;
double cosine;
SAMPLE testData[N];

//Para definir punto fijo de 32 bits.
int FIXED_POINT_16 = 16;
int ONE_16 = 1 << FIXED_POINT_16;

int FIXED_POINT_30 = 30;
int ONE_30 = 1 << FIXED_POINT_30;

int toFix( float val, int ONE ) {
    // Escalamiento
    return (int) (val * ONE);
}

float floatVal( int fix, int ONE ) {
    return ((float) fix) / ONE;
}

int intVal( int fix, int FIXED_POINT ) {
    return fix >> FIXED_POINT;
}

int mul(int a, int b, int FIXED_POINT_mixed) {
    // Manejo de 64 bit para el resultado inmedianto de la multiplicación
    // Conversion a 32 bits para posterior uso
    return (int)((long long int)fix_coeff * (long long int)Q1 >> FIXED_POINT_mixed);
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
    fix_coeff = toFix(coeff, ONE_30);
    printf("For SAMPLING_RATE = %f", SAMPLING_RATE);
    printf(" N = %d", N);
    printf(" and FREQUENCY = %f,\n", TARGET_FREQUENCY);
    printf("k = %d and coeff = %f\n\n", k, coeff);
    ResetGoertzel();
}

/* Call this routine for every sample. */
void ProcessSample(SAMPLE sample)
{
    // Punto fijo 32. INT
    int Q0;
    Q0 = mul(fix_coeff, Q1, 30) - Q2 + toFix(sample, ONE_16);
    Q2 = Q1;
    Q1 = Q0;
}

/* Optimized Goertzel */
/* Call this after every block to get the RELATIVE magnitude squared. */
float GetMagnitudeSquared(void)
{
    float result;
    result = floatVal(Q1, ONE_16) * floatVal(Q1,ONE_16) + floatVal(Q2,ONE_16) * floatVal(Q2,ONE_16) - floatVal(Q1,ONE_16) * floatVal(Q2,ONE_16) * coeff;
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
    return 0;
}
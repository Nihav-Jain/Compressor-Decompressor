#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#define OUTPUT_BIN_FILENAME "../verts_compressed.bin"
#define NUM_BITS_PER_BYTE 8

int getNextXYZvalues(FILE* filePtr, double *x, double *y, double *z);
double maxDouble(double a, double b);
double minDouble(double a, double b);
int compress(double value, double minValue, double segmentLength);

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Invalid command.\nCommand format: Compressor.exe <no. of compression bits> <path to input data file>\n");
		return 0;
	}

	int compressionBits = atoi(argv[1]);
	int totalPossibleValues = (int)pow(2, compressionBits) - 1;

	FILE* inputDataFile;
	fopen_s(&inputDataFile, argv[2], "r");
	//FILE* outputBinFile = fopen(OUTPUT_BIN_FILENAME, "wb");

	//printf("%d\n", NUM_BITS_PER_BYTE * sizeof(int));

	double x, y, z;
	double minValue = DBL_MAX;
	double maxValue = DBL_MIN;
	int dataCount = 0;

	int fileNotEnded;
	do
	{
		fileNotEnded = getNextXYZvalues(inputDataFile, &x, &y, &z);
		dataCount++;
		maxValue = maxDouble(maxDouble(x, y), maxDouble(z, maxValue));
		minValue = minDouble(minDouble(x, y), minDouble(z, minValue));
	} while (fileNotEnded);
	fclose(inputDataFile);

	double segmentLength = (maxValue - minValue) / totalPossibleValues;
	fopen_s(&inputDataFile, argv[2], "r");
	do
	{
		fileNotEnded = getNextXYZvalues(inputDataFile, &x, &y, &z);
		printf("%lf %lf %lf\n", x, y, z);
		printf("%d %d %d\n", compress(x, minValue, segmentLength), compress(y, minValue, segmentLength), compress(z, minValue, segmentLength));

	} while (fileNotEnded);
	fclose(inputDataFile);

	//fclose(outputBinFile);

	return 0;
}

int getNextXYZvalues(FILE* filePtr, double *x, double *y, double *z)
{
	int id;
	fscanf_s(filePtr, "%d: %lf %lf %lf", &id, x, y, z);
	//printf("%d: %lf %lf %lf\n", id, *x, *y, *z);

	char c;
	do
	{
		c = fgetc(filePtr);
		//printf("%c", c);
		if (c == EOF)
			return 0;
	} while (c != '\n');
	if (c == EOF)
		return 0;
	//printf("\n");
	return 1;
}

double maxDouble(double a, double b)
{
	return (a > b) ? a : b;
}

double minDouble(double a, double b)
{
	return (a < b) ? a : b;
}

int compress(double value, double minValue, double segmentLength)
{
	int n = (int)((value - minValue) / segmentLength);	// not adding 1 because compressed value (n) will start from 0
	return n;
}
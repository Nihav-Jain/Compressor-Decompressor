#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#define OUTPUT_BIN_FILENAME "../verts_compressed.bin"
#define NUM_BITS_PER_BYTE 8

unsigned short compress(double value, double minValue, double segmentLength);
int getNextValue(FILE* filePtr, double *value);

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

	//printf("%d\n", NUM_BITS_PER_BYTE * sizeof(short));

	double inputValue;
	double minValue = DBL_MAX;
	double maxValue = DBL_MIN;
	int dataCount = 0;

	int fileNotEnded;
	do
	{
		fileNotEnded = getNextValue(inputDataFile, &inputValue);
		dataCount++;
		maxValue = max(inputValue, maxValue);
		minValue = min(inputValue, minValue);
		//printf("%lf\n", inputValue);
	} while (fileNotEnded);
	fclose(inputDataFile);

	double segmentLength = (maxValue - minValue) / totalPossibleValues;
	fopen_s(&inputDataFile, argv[2], "r");
	do
	{
		fileNotEnded = getNextValue(inputDataFile, &inputValue);
		printf("%lf %d\n", inputValue, compress(inputValue, minValue, segmentLength));

	} while (fileNotEnded);
	fclose(inputDataFile);

	//fclose(outputBinFile);

	return 0;
}

unsigned short compress(double value, double minValue, double segmentLength)
{
	unsigned short n = (int)((value - minValue) / segmentLength);	// not adding 1 because compressed value (n) will start from 0
	return n;
}

int getNextValue(FILE* filePtr, double *value)
{
	static int count = 0;
	int id;
	char c;
	int returnVal = 1;
	if (count == 0)
	{
		fscanf_s(filePtr, "%d:", &id);
	}
	fscanf_s(filePtr, "%lf", value);
	if (count == 2)
	{
		do
		{
			c = fgetc(filePtr);
			if (c == EOF)
				returnVal = 0;
		} while (c != '\n' && c != EOF);
	}
	count = (count + 1) % 3;
	return returnVal;
}

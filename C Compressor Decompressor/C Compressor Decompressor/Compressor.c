#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <malloc.h>

#define OUTPUT_BIN_FILENAME "../verts_compressed.bin"
#define OUTPUT_TXT_FILENAME "../verts_orig_data.txt"
#define NUM_BITS_PER_BYTE 8

unsigned short compress(double value, double minValue, double segmentLength);
int getNextValue(FILE* filePtr, double *value);
char getKthBitOfNumber(unsigned short number, unsigned short k, int* bitMasks);

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Invalid command.\nCommand format: Compressor.exe <no. of compression bits> <path to input data file>\n");
		return 0;
	}

	int compressionBits = atoi(argv[1]);
	int totalPossibleValues = (int)pow(2, compressionBits) - 1;

	int *bitMasks = (int *)malloc(compressionBits * sizeof(int));
	int i;
	int powerOf2 = 2;
	bitMasks[0] = 1;
	for (i = 1; i < compressionBits; i++)
	{
		bitMasks[i] = powerOf2; //bitMasks[i - 1] + powerOf2;
		powerOf2 *= 2;
	}

	FILE* inputDataFile;
	fopen_s(&inputDataFile, argv[2], "r");
	FILE* outputBinFile;
	fopen_s(&outputBinFile, OUTPUT_BIN_FILENAME, "wb");
	FILE* outputTxtFile;
	fopen_s(&outputTxtFile, OUTPUT_TXT_FILENAME, "w");

	printf("%d\n", NUM_BITS_PER_BYTE * sizeof(double));

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
	} while (fileNotEnded);
	fclose(inputDataFile);

	fwrite(&compressionBits, sizeof(int), 1, outputBinFile);
	fwrite(&dataCount, sizeof(int), 1, outputBinFile);
	fwrite(&minValue, sizeof(double), 1, outputBinFile);
	fwrite(&maxValue, sizeof(double), 1, outputBinFile);

	double segmentLength = (maxValue - minValue) / totalPossibleValues;
	char charToWrite = 0, kthBit;
	int carry = 0, charBitPtr = 7, bitsLeft = 0;
	unsigned short compressedValue;

	fopen_s(&inputDataFile, argv[2], "r");
	for (i = 0; i < dataCount; i++)
	{
		getNextValue(inputDataFile, &inputValue);
		compressedValue = compress(inputValue, minValue, segmentLength);
		printf("%lf %d\n", inputValue, compressedValue);
		bitsLeft = compressionBits - 1;
		while (bitsLeft >= 0)
		{
			kthBit = getKthBitOfNumber(compressedValue, bitsLeft, bitMasks);
			printf("%d", kthBit);
			kthBit = kthBit << charBitPtr;
			charToWrite = charToWrite | kthBit;
			charBitPtr--;
			if (charBitPtr < 0)
			{
				charBitPtr = 7;
				fwrite(&charToWrite, sizeof(char), 1, outputBinFile);
				charToWrite = 0;
			}
			bitsLeft--;
		}
		printf("\n");
		fprintf(outputTxtFile, "%lf\n", inputValue);
	}
	if (charBitPtr >= 0)
	{
		fwrite(&charToWrite, sizeof(char), 1, outputBinFile);
	}

	fclose(inputDataFile);
	fclose(outputBinFile);
	fclose(outputTxtFile);

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

// k is 0 for LSB
char getKthBitOfNumber(unsigned short number, unsigned short k, int* bitMasks)
{
	number = number & bitMasks[k];
	number = number >> k;
	return   (char)number;
}
/*
 *	Execute Command:	Compressor.exe <no. of compression bits> <path to input data file>
 *	Takes in the numbner of bits to be used for comressing each data and the path to the file containing the data to be compressed as command line arguments
 *	Ouput:
 *	1. A binary file containig -
 *		a. Long word representing the number of bits to used for compression
 *		b. Long word representing the number of data values which were comrpessed
 *		c. Long word representing the (double) minimum value among the input data values
 *		d. Long word representing the (double) maximum value among the input data values
 *		e. Series of bytes representing all the compressed values
 *	2. A text file containing the original data values which were compressed (this text file will be used by the decompressor to calculate the error)
 */

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <malloc.h>

#define OUTPUT_BIN_FILENAME "verts_compressed.bin"
#define OUTPUT_TXT_FILENAME "verts_orig_data.txt"
#define NUM_BITS_PER_BYTE 8

#define NO_OF_COMPRESSION_BITS 1
#define PATH_TO_INPUT_DATA_FILE 2

unsigned int compress(double value, double minValue, double segmentLength);
int getNextValue(FILE* filePtr, double *value);
char getKthBitOfNumber(unsigned int number, unsigned short k, unsigned int* bitMasks);

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Invalid command.\nCommand format: Compressor.exe <no. of compression bits> <path to input data file>\n");
		return 0;
	}

	int compressionBits = atoi(argv[NO_OF_COMPRESSION_BITS]);									// first command line argument is the number of bits to be used for compression
	int totalPossibleValues = (int)pow(2, compressionBits);										// possible values after compression = 2^bits

	unsigned int *bitMasks = (unsigned int *)malloc(compressionBits * sizeof(unsigned int));
	int i;
	unsigned int powerOf2 = 2;
	bitMasks[0] = 1;
	for (i = 1; i < compressionBits; i++)
	{
		bitMasks[i] = powerOf2;																	// bit mask for the i'th LSB is 2^i
		powerOf2 *= 2;
	}

	FILE* inputDataFile;
	fopen_s(&inputDataFile, argv[PATH_TO_INPUT_DATA_FILE], "r");

	if (inputDataFile == NULL)
	{
		printf("File not found %s\n", argv[PATH_TO_INPUT_DATA_FILE]);
		return 0;
	}

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

	FILE* outputBinFile;
	fopen_s(&outputBinFile, OUTPUT_BIN_FILENAME, "wb");
	FILE* outputTxtFile;
	fopen_s(&outputTxtFile, OUTPUT_TXT_FILENAME, "w");

	fwrite(&compressionBits, sizeof(int), 1, outputBinFile);
	fwrite(&dataCount, sizeof(int), 1, outputBinFile);
	fwrite(&minValue, sizeof(double), 1, outputBinFile);
	fwrite(&maxValue, sizeof(double), 1, outputBinFile);

	double segmentLength = (maxValue - minValue) / totalPossibleValues;							// data range for each compression bucket
	char charToWrite = 0, kthBit;
	int charBitPtr = NUM_BITS_PER_BYTE - 1, bitsLeft;
	unsigned int compressedValue;

	fopen_s(&inputDataFile, argv[PATH_TO_INPUT_DATA_FILE], "r");
	for (i = 0; i < dataCount; i++)
	{
		getNextValue(inputDataFile, &inputValue);

		compressedValue = compress(inputValue, minValue, segmentLength);						// get compressed value
		if (compressedValue == totalPossibleValues)												// special condition for max input value since its compressed value overflows the number of compression bits
			compressedValue--;

		bitsLeft = compressionBits - 1;															// bitsLeft holds the number of bits of the current compressed value left to write in the bin file
		while (bitsLeft >= 0)
		{
			kthBit = getKthBitOfNumber(compressedValue, bitsLeft, bitMasks);
			kthBit = kthBit << charBitPtr;														// shift the kth bit of compressed value to the place it will be positioned in the byte to be written in the bin file
			charToWrite = charToWrite | kthBit;
			charBitPtr--;
			if (charBitPtr < 0)																	// if we have assigned all 8 bits of the byte to write, then write it on the file and reinitialize the variables
			{
				charBitPtr = NUM_BITS_PER_BYTE - 1;
				fwrite(&charToWrite, sizeof(char), 1, outputBinFile);
				charToWrite = 0;
			}
			bitsLeft--;
		}
		fprintf(outputTxtFile, "%lf\n", inputValue);											// writing original value to a stats file
	}
	if (charBitPtr >= 0)																		// special condition to write last compressed value
	{																							// occurs when the 8 bits of the byte to write have not been fully filled but all the bits of the compressed value have been assigned to the byte
		fwrite(&charToWrite, sizeof(char), 1, outputBinFile);
	}

	fclose(inputDataFile);
	fclose(outputBinFile);
	fclose(outputTxtFile);

	printf("Compression successful.\n");

	return 0;
}

/**
 *	@method	compress
 *	@desc	takes in the value to compress, the min possible value of the input data set and the range of one compression bucket
 *			Using formula for arithmetic progression => n = (An - A0) / d	(not adding 1 because compressed value (n) will start from 0)
 *	@return the compressed value
 */
unsigned int compress(double value, double minValue, double segmentLength)
{
	unsigned int n = (unsigned int)(((value - minValue) / segmentLength));
	return n;
}

/**
*	@method	getNextValue
*	@desc	takes in the file pointer to read from and the pointer to place the input into
*	@return 0 or 1 to represent if end of file has been reached
*/
int getNextValue(FILE* filePtr, double *value)
{
	static int count = 0;							// to keep track of which value is being read from the file; if count is 0, 1 or 2 we are reading the desired value
	int id;
	char c;
	int returnVal = 1;
	if (count == 0)									// getting the first integer (id/index) of the input file out of the way
	{
		fscanf_s(filePtr, "%d:", &id);
	}
	fscanf_s(filePtr, "%lf", value);				// reading the desired double value
	if (count == 2)
	{
		do
		{
			c = fgetc(filePtr);
			if (c == EOF)
				returnVal = 0;
		} while (c != '\n' && c != EOF);			// read character by character until end of line of end of file
	}
	count = (count + 1) % 3;
	return returnVal;
}

/**
*	@method	getKthBitOfNumber
*	@desc	takes in the number whose bit value is to be returned, the bit index and the array of bit masks
*	@return 1 or 0 depending on whether the kth bit of the number is set or not
*/
char getKthBitOfNumber(unsigned int number, unsigned short k, unsigned int* bitMasks)
{
	number = number & bitMasks[k];
	number = number >> k;
	return   (char)number;
}
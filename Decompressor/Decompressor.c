/*
*	Execute Command:	Decompressor.exe <path to compressed binary file> <path to original data file>
*	Takes in the path to the compressed file and the path to the stats file containing the original data as command line arguments
*	Ouput:	Root mean square error between the original data and the compressed data
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_BITS_PER_BYTE 8
#define PATH_TO_BIN_FILE 1
#define PATH_TO_ORIG_DATA_FILE 2

double decompress(unsigned int value, double minValue, double segmentLength);
char getKthBitOfNumber(char number, unsigned int k, int* bitMasks);

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("Invalid command.\nCommand format: Decompressor.exe <path to compressed binary file> <path to original data file>\n");
		return 0;
	}

	FILE* compressedFile;
	fopen_s(&compressedFile, argv[PATH_TO_BIN_FILE], "rb");

	FILE* origDataFile;
	fopen_s(&origDataFile, argv[PATH_TO_ORIG_DATA_FILE], "r");

	int compressionBits, dataCount;
	fread(&compressionBits, sizeof(int), 1, compressedFile);
	fread(&dataCount, sizeof(int), 1, compressedFile);

	double minValue, maxValue;
	fread(&minValue, sizeof(double), 1, compressedFile);
	fread(&maxValue, sizeof(double), 1, compressedFile);

	int totalPossibleValues = (int)pow(2, compressionBits);										// possible values after compression = 2^bits
	double segmentLength = (maxValue - minValue) / totalPossibleValues;							// data range for each compression bucket
	minValue = minValue + segmentLength / 2;													// taking average of the segment ranges for better decompression accuracy


	int i;
	int *bitMasks = (int *)malloc(NUM_BITS_PER_BYTE * sizeof(int));								// to store bit masks for the 8 bits of a byte
	int powerOf2 = 2;
	bitMasks[0] = 1;
	for (i = 1; i < NUM_BITS_PER_BYTE; i++)
	{
		bitMasks[i] = powerOf2;																	// bit mask for the i'th LSB is 2^i
		powerOf2 *= 2;
	}

	unsigned int compressedValue = 0;
	char inputChar, kthBit;
	int charBitPtr, bitsLeft = compressionBits - 1;
	int numberOfChars = (int)ceil(compressionBits * dataCount / 8.0f);							// number of character in the compressed file containing the comressed values
	unsigned int temp;
	double decompressedValue, originalData, sumOfSqDiff = 0;
	charBitPtr = -1;

	for (i = 0; i < dataCount; i++)
	{
		bitsLeft = compressionBits - 1;															// bitsLeft holds the number of bits of the current compressed value left to read from teh file
		compressedValue = 0;																	// to hold the compressed value stored in the file
		while (bitsLeft >= 0)
		{
			if (charBitPtr < 0)																	// read the next character from the file if we run out of bits from the current one
			{
				charBitPtr = NUM_BITS_PER_BYTE - 1;
				fread(&inputChar, sizeof(char), 1, compressedFile);
			}

			kthBit = getKthBitOfNumber(inputChar, charBitPtr, bitMasks);						// get the kth bit of the current character read from the file
			temp = kthBit & 0x0001;																// done to handle the case where C sign extends the bit value
			temp = temp << bitsLeft;															// position the bit to where it will go in the compressed value
			compressedValue = compressedValue | temp;											// construct the compressed value

			bitsLeft--;
			charBitPtr--;
		}

		decompressedValue = decompress(compressedValue, minValue, segmentLength);				// interpret the original value from the compressed value
		fscanf_s(origDataFile, "%lf", &originalData);

		originalData -= decompressedValue;														// difference between original value and interpreted decompresed value
		sumOfSqDiff += (originalData * originalData);											// sum of square of the difference
	}

	double RMSD = sqrt(sumOfSqDiff / dataCount);												// root mean square error
	printf("Root mean square error for a %d bit compression = %lf\n", compressionBits, RMSD);

	fclose(compressedFile);
	fclose(origDataFile);
	return 0;
}

/**
*	@method	decompress
*	@desc	takes in the value to decompress, the min possible value of the input data set and the range of one compression bucket
*			Using formula for arithmetic progression => An = A0 + n * d	(not using (n-1)*d because compressed value (n) will start from 0)
*	@return the decompressed value
*/
double decompress(unsigned int value, double minValue, double segmentLength)
{
	return minValue + value * segmentLength;		// An = A0 + n * d;
}

/**
*	@method	getKthBitOfNumber
*	@desc	takes in the number whose bit value is to be returned, the bit index and the array of bit masks
*	@return 1 or 0 depending on whether the kth bit of the number is set or not
*/
char getKthBitOfNumber(char number, unsigned int k, int* bitMasks)
{
	number = number & bitMasks[k];
	number = number >> k;
	return number;
}
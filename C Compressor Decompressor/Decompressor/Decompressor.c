#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_BITS_PER_BYTE 8

double decompress(unsigned short value, double minValue, double segmentLength);
char getKthBitOfNumber(char number, unsigned short k, int* bitMasks);

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Invalid command.\nCommand format: Decompressor.exe <path to compressed binary file> <path to original data file>\n");
		return 0;
	}

	FILE* compressedFile;
	fopen_s(&compressedFile, argv[1], "rb");

	FILE* origDataFile;
	fopen_s(&origDataFile, argv[2], "r");
	
	int compressionBits, dataCount;
	fread(&compressionBits, sizeof(int), 1, compressedFile);
	fread(&dataCount, sizeof(int), 1, compressedFile);

	double minValue, maxValue;
	fread(&minValue, sizeof(double), 1, compressedFile);
	fread(&maxValue, sizeof(double), 1, compressedFile);

	int totalPossibleValues = (int)pow(2, compressionBits);
	double segmentLength = (maxValue - minValue) / totalPossibleValues;
	minValue = minValue + segmentLength / 2;		// taking average of the segment ranges


	int i;
	int *bitMasks = (int *)malloc(NUM_BITS_PER_BYTE * sizeof(int));
	int powerOf2 = 2;
	bitMasks[0] = 1;
	for (i = 1; i < NUM_BITS_PER_BYTE; i++)
	{
		bitMasks[i] = powerOf2; //bitMasks[i - 1] + powerOf2;
		powerOf2 *= 2;
	}

	unsigned short compressedValue = 0;
	char inputChar, kthBit;
	int charBitPtr, bitsLeft = compressionBits-1;
	int numberOfChars = (int) ceil(compressionBits * dataCount / 8.0f);
	unsigned short temp;
	double decompressedValue, originalData, sumOfSqDiff = 0;
	charBitPtr = -1;

	for (i = 0; i < dataCount; i++)
	{
		bitsLeft = compressionBits - 1;
		compressedValue = 0;
		while (bitsLeft >= 0)
		{
			if (charBitPtr < 0)
			{
				charBitPtr = NUM_BITS_PER_BYTE - 1;
				fread(&inputChar, sizeof(char), 1, compressedFile);
				//printf("%X\n", inputChar);
			}
			kthBit = getKthBitOfNumber(inputChar, charBitPtr, bitMasks);
			temp = kthBit & 0x0001;
			//printf("%d", temp);
			temp = temp << bitsLeft;
			compressedValue = compressedValue | temp;
			bitsLeft--;
			charBitPtr--;
		}
		decompressedValue = decompress(compressedValue, minValue, segmentLength);
		//printf(" %d %lf\n", compressedValue, decompressedValue);
		//printf("\n");
		fscanf_s(origDataFile, "%lf", &originalData);
		originalData -= decompressedValue;
		sumOfSqDiff += (originalData * originalData);
	}

	double RMSD = sqrt(sumOfSqDiff / dataCount);
	printf("Root mean square error for a %d bit compression = %lf\n", compressionBits, RMSD);

	fclose(compressedFile);
	fclose(origDataFile);
	return 0;
}

double decompress(unsigned short value, double minValue, double segmentLength)
{
	return minValue + value * segmentLength;		// An = A0 + n * d;
}

// k is 0 for LSB
char getKthBitOfNumber(char number, unsigned short k, int* bitMasks)
{
	number = number & bitMasks[k];
	number = number >> k;
	return number;
}
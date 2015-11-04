#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUTPUT_BIN_FILENAME "../verts_comrpessed.bin"

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Invalid command.\nCommand format: Compressor.exe <no. of compression bits> <path to input data file>\n");
		return 0;
	}

	int compressionBits = atoi(argv[1]);
	FILE* inputDataFile = fopen(argv[2], "r");
	FILE* outputBinFile = fopen(OUTPUT_BIN_FILENAME, "wb");
	//printf("Compression bits = %d, File name = %s\n", compressionBits, argv[2]);
	if (inputDataFile == NULL)
		printf("input file is null");
	else
		fclose(inputDataFile);
	fclose(outputBinFile);

	return 0;
}
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Invalid command.\nCommand format: Decompressor.exe <path to compressed binary file> <path to original data file>\n");
		return 0;
	}

	FILE* compressedFile;
	fopen_s(&compressedFile, argv[1], "rb");
	
	int compressionBits, dataCount;
	fread(&compressionBits, sizeof(int), 1, compressedFile);
	fread(&dataCount, sizeof(int), 1, compressedFile);
	printf("%d %d\n", compressionBits, dataCount);

	double minValue, maxValue;
	fread(&minValue, sizeof(double), 1, compressedFile);
	fread(&maxValue, sizeof(double), 1, compressedFile);
	printf("%lf %lf\n", minValue, maxValue);



	fclose(compressedFile);
	return 0;
}

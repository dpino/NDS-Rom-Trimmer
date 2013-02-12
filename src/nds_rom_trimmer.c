/**
 * Copyright (C) 2008  Diego Pino Garcia  <dpino@igalia.com>

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/**
 * Simple utility programm to trim a NDS ROM file
 *
 * The file is read backwards, reading its content in chunks of BUFFER_SIZE size. 
 * For each chunk scanned, if it contains data that is not padding data, 
 * that means at least one byte in the chunk is not equal to 0xFF. That precise segment of 
 * data in the file indicates the end of the real data, meaning that it's possible to truncate 
 * the file at that point
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define EXTENSION 	".nds"
#define BUFFER_SIZE 4096
#define MAX_LENGTH 	256 
#define MB_SIZE 	1048576
#define KB_SIZE 	1024

void help();
void print_info(int filesize, int realsize);

int main(int argc, char* argv[]) {
	FILE* stream;
	char buffer[BUFFER_SIZE];
	char *pos;
	int filesize, fpos, total; 
	int realsize = 0, count = 0, i;
	char *filename;

	if (argc < 2) {
		help();
	}
	filename = argv[1];

	if (!strstr(filename, EXTENSION)) {
		help();
	}

	stream = fopen(filename, "rb");
	if (stream == NULL) {
		printf("Couldn't open %s\n", filename);
		exit(-1);
	}

	filesize = getFileSize(stream);
	if (filesize == 0) {
		printf("Error while reading %s: filesize is 0\n", filename);
		exit(-1);
	}

	fpos = ftell(stream);
	fseek(stream, filesize - (BUFFER_SIZE - 1), SEEK_SET);

		// Start scanning backwards
	printf("Trimming %s(%dMB%dKB) to...", filename, (filesize / MB_SIZE), filesize % KB_SIZE);
	while (fpos >= 0) {
		fread((char*)buffer, sizeof(char), (BUFFER_SIZE - 1), stream);
		buffer[BUFFER_SIZE] = '\0';
		pos = buffer; count = 0;

			// Sum up all the chunks equal to 0xFF (padding bytes)
		while (pos != NULL && *pos != '\0') {
			if (!(*pos ^ 0xFF) == 0) {
				count++;
			}
		
			pos++;
		}

			// If there was at least a chunk not equal to 0xFF
		if (count != (BUFFER_SIZE - 1)) {
			fseek(stream, count, SEEK_CUR);
			realsize = ftell(stream);
			break;
		}

			// Move back to new piece of data about to read
		total += BUFFER_SIZE - 1;
		fseek(stream, filesize - total, SEEK_SET);
		fpos = ftell(stream);
	}

    if (realsize > filesize) {
        realsize = filesize;
    }

    print_info(filesize, realsize);

		// Trim file to newfile
	trim(stream, filename, realsize);

	fclose(stream);

	return 0;
}

void help()
{
	printf("nds-rom-trimmer <filename.nds>\n");
	exit(-1);
}

int getFileSize(FILE* stream)
{
	int filesize = 0;

	fseek(stream, 0, SEEK_END);
	filesize = ftell(stream);
	fseek(stream, 0, SEEK_SET);

	return filesize;
}

/**
 * Get rid of the whatever data in the file after filesize, storing meaningful data (that means from 0 to filesize) in a new file
 *
 */ 
int trim(FILE* strin, char* filein, int filesize)
{
	FILE* strout;
	char fileout[MAX_LENGTH], buffer[BUFFER_SIZE];
	int pos, fpos, newsize;

	pos = mindex(filein, EXTENSION);
	strncpy(fileout, filein, pos);
        fileout[pos] = '\0';
	strcat(fileout, "_trim");
	strcat(fileout, EXTENSION);

	strout = fopen(fileout, "wb+");
	if (!strout) {
		printf("Couldn't open %s for write\n", fileout);
		exit(-1);
	}

	fseek(strin, 0, SEEK_SET);
	for (fpos = 0; fpos < filesize; fpos = ftell(strin)) {
		fread((char*)buffer, sizeof(char), BUFFER_SIZE, strin);
		fwrite((char*)buffer, sizeof(char), BUFFER_SIZE, strout);
	}

	newsize = ftell(strout);
	fclose(strout);

	printf("%s(%dMB%dKB)\n", fileout, (newsize/ MB_SIZE), newsize % KB_SIZE);

	return newsize;
}

int mindex(char* haystack, char* needle)
{
	char *pos, *npos;
	int count;
	
	pos = strstr(haystack, needle);
	if (pos != NULL) {
		for (npos = haystack, count = 0; npos != pos; npos++) count++;
	}

	return count;
}

/**
 * Prints summary information about how much the file was trimmed
 *
 */
void print_info(int filesize, int realsize) {
    printf("Trimmed from %d to %d", filesize, realsize);
}

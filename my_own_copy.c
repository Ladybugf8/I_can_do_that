/* 
 * Donna Enke
 * my_own_copy.c
 * writes a program to copy a source file to a destination file
 */

#include <stdio.h>
#include <stdint.h>

typedef uint8_t BYTE;

int main (int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 3)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }
    
    // open input file
    FILE* src = fopen(argv[1], "r");
    if (src == NULL)
    {        
        printf("Could not open input file. \n");

        return 2;
    }
   
    // open output file
    FILE* dst = fopen(argv[2], "w");
    if (dst == NULL)
    {
        fclose(src);
        fprintf(stderr, "Could not create output file.\n");
        return 3;
    }
    
    BYTE buffer;

    // read until there are no more bytes
    // this works because at EOF fread returns 0 so the expression evaluates as false
    while (fread(&buffer, sizeof(BYTE), 1, src))
    {
        //write to the file
        fwrite(&buffer, sizeof(BYTE), 1, dst);
    }
    
    // close the files
    fclose(src);
    fclose(dst);
    
    return 0;
}

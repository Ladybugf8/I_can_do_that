/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Written by: Donna Enke
 * March 16, 2014
 *
 * Recovers JPEGs from a forensic image.
 */

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

// define a byte of data
typedef uint8_t BYTE;

// define functions

int main(void)
{
    // I may want to make this a global variable
    int jpg_name = 0;
    
    // I want an array to hold 512 bytes of info
    BYTE buffer[512];
    
    // To avoid having to do a cast each time I check, I'm setting up an array to hold the jpeg values
    BYTE jpg_start[] = {0xff,0xd8,0xff};
    BYTE jpg_4[] = {0xe0,0xe1};
    
    // Boolean to determine initial read
    bool first_file = false;
    
    // TODO - cast jpg_name to char, evaluate if less than 10, add 00 else add 0 then output to filename
    
    // open input file
    FILE* src = fopen("card.raw", "r");
    if (src == NULL)
    {        
        printf("Could not open card.raw file. \n");

        return 1;
    }
   
    // open output file - put this in a function
    FILE* dst = fopen("000.jpg", "w");
    if (dst == NULL)
    {
        fclose(src);
        fprintf(stderr, "Could not create 000.jpg file.\n");
        return 2;
    }
    
    // priming read to find the start of the first jpg   
    do
    {
        fread(&buffer, sizeof(BYTE), 512, src);
        if (buffer[0] == jpg_start[0] && buffer[1] == jpg_start[1] & buffer[2] == jpg_start[2])
        {
            //evaluate 4th byte
            if (buffer[3] == jpg_4[0] || buffer[3] == jpg_4[1])
            {
                // congratulations, you found the start of the first file
                first_file = true;
                printf("Found start of first file. First file = %s! \n", first_file ? "true" : "false");
             } 
            else
                printf("Priming Read - First 3 bytes matched, 4 byte didn't. \n");
        }
        else
            printf("Priming Read - First 3 bytes did not match. \n");  
    }
    while (!first_file);
    
    /**
     * At this point, I have opened the first output file
     * and read the first byte of a jpg. 
     * Now I print out the first first file
     * I want to create functions to:
     *   1) create output file file name (incrementing number)
     *   2) write buffer to output file until new file
     *   3) close current output file
     */
        
    // read input file until there are no more bytes
    // this works because at EOF fread returns 0 so the expression evaluates as false
    while (fread(&buffer, sizeof(BYTE), 512, src))
    {
        // if I understand this correctly, I just read into an array named buffer
        // which has 512 segments of 1 byte each
        
        if (buffer[0] == jpg_start[0] && buffer[1] == jpg_start[1] & buffer[2] == jpg_start[2])
        {
            //evaluate 4th byte
            if (buffer[3] == jpg_4[0] || buffer[3] == jpg_4[1])
                // congratulations, you found the start of a new file
                // pass buffer to function that writes to a file
                printf("Time to write function.\n");
                jpg_name ++;
         }
        else
        {
            // this is the priming read, just ignore what you've read
            printf("Reading 512 bytes at a time. \n");
        }
    }
    return 0;
}   

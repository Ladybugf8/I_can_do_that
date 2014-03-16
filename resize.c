/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Resizes a BMP per user input.
 */
       
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "bmp.h"

int main(int argc, char* argv[])
{   
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }
    
    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];
        
    // check that argv[1] is digits
    int resize;
    
    if (isdigit((unsigned char) *argv[1]))
        resize = atoi(argv[1]);
    else
        return 5;

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf_in;
    fread(&bf_in, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi_in;
    fread(&bi_in, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf_in.bfType != 0x4d42 || bf_in.bfOffBits != 54 || bi_in.biSize != 40 || 
        bi_in.biBitCount != 24 || bi_in.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // declare output file header
    BITMAPFILEHEADER bf_out;
    
    // declare output info header
    BITMAPINFOHEADER bi_out;

    // Create outfile's file header
    bf_out.bfType = bf_in.bfType;
    // Since you square what's left, gotta take out the file headers, then square, then put headers back in
    bf_out.bfSize = ((bf_in.bfSize - 80) * resize * resize) + 80; 
    bf_out.bfReserved1 = bf_in.bfReserved1;
    bf_out.bfReserved2 = bf_in.bfReserved2;
    bf_out.bfOffBits = bf_in.bfOffBits;
    
    // Create outfile's info header
    bi_out.biSize = bi_in.biSize;

    // This is in pixels so padding shouldn't matter
    bi_out.biWidth = bi_in.biWidth * resize;

    // This is also in pixels
    bi_out.biHeight = bi_in.biHeight * resize;
    
    bi_out.biPlanes = bi_in.biPlanes;
   
    // Gotta remember padding here
    if ((bi_in.biBitCount * resize) % 4 == 0)
        bi_out.biBitCount = bi_in.biBitCount * resize;
    else
    {
        int needs_padding = (bi_in.biBitCount * resize) % 4;
        int amt_to_pad = 4-needs_padding;
        bi_out.biBitCount = bi_in.biBitCount * resize + amt_to_pad;
    }
    bi_out.biCompression = bi_in.biCompression;
    bi_out.biSizeImage = bf_out.bfSize;
    bi_out.biXPelsPerMeter = bi_in.biXPelsPerMeter;
    bi_out.biYPelsPerMeter = bi_in.biXPelsPerMeter;
    bi_out.biClrUsed = bi_in.biClrUsed;
    bi_out.biClrImportant = bi_in.biClrImportant;
    
        
           
 
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf_out, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi_out, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines of INPUT FILE
    int in_padding =  (4 - (bi_in.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // determine padding for scanlines of OUTPUT FILE
    int out_padding =  (4 - (bi_out.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over infile's scanlines -- THIS IS THE LINE
    for (int i = 0, biHeight = abs(bi_in.biHeight); i < biHeight; i++)
    { 
        // set up buffer to remember each line's pixels
        RGBTRIPLE line_storage[bi_in.biWidth];
                   
        // iterate over pixels in scanline -- THIS IS EACH INDIVIDUAL PIXEL
        for (int j = 0; j < bi_in.biWidth; j++)
        {
            // read RGB triple from infile
            fread(&line_storage[j], sizeof(RGBTRIPLE), 1, inptr);     
        }
        // skip over padding, if any, remember that padding is not included in biWidth
        fseek(inptr, in_padding, SEEK_CUR);
        
        // at this point you are still inside the line read loop and have read in all the pixels
        // each pixel has been put into an array named line_storage
        // now it is time to write the pixels into the output file        
        for (int line = 0; line < resize; line++)
        {
        
            // write RGB triple to outfile RESIZE number of times
            for (int pixel_count = 0; pixel_count < bi_in.biWidth; pixel_count++)
            {
                for (int rewrite = 0; rewrite < resize; rewrite++)
                    fwrite(&line_storage[pixel_count], sizeof(RGBTRIPLE), 1, outptr);          
            }
            // add padding (if necessary)
            for (int k = 0; k < out_padding; k++)
                 fputc(0x00, outptr);   
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}


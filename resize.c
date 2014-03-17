/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Written by: Donna Enke
 * March 16, 2014
 * 
 * Resizes a BMP per user input.
 * 
 * A hearty THANK YOU!!! to stackoverflow.com/questions/3950846/isdigit-segmentation-fault
 * for helping me figure out the cause of my segmentation fault at runtime
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
        
    // check that argv[1] is digits, convert to integer, and remember value
    int resize;
    
    if (isdigit((unsigned char) *argv[1]))
        resize = atoi(argv[1]);
    else
        return 2;
    
    //ensure resize is within scope    
    if (resize < 1 || resize > 100)
    {   
        printf("This program is limited to resize values between 1 and 100 inclusive. \n");
        printf("You entered %d!!! \n", resize);
        return 3;         
    }   

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 4;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 5;
    }
    
    // at this point, we know the arguments are valid
    
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
        return 6;
    }
    
    // determine padding (in bytes) for scanlines of INPUT FILE
    int in_padding =  (4 - (bi_in.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // declare output file header
    BITMAPFILEHEADER bf_out;
    
    // declare output info header
    BITMAPINFOHEADER bi_out;
    
    /**
     * Create outfile's info header 
     * Since I use some of the information calculated in the bi_header
     * for the bf_header, I create this first. 
     */
 
    //this is just the number of bytes used by the header, it doesn't change
    bi_out.biSize = bi_in.biSize;

    // This is in pixels and does not include padding 
    bi_out.biWidth = bi_in.biWidth * resize;

    // This is also in pixels
    bi_out.biHeight = bi_in.biHeight * resize;
    
    // doesn't change
    bi_out.biPlanes = bi_in.biPlanes;
   
    // This is the number of bits per pixel, it doesn't change
    bi_out.biBitCount = bi_in.biBitCount;

    // doesn't change
    bi_out.biCompression = bi_in.biCompression;

    /**
     * Gotta do math for biSizeImage
     * This is the total size of the image
     * including pixels AND padding
     */
     
    // figure out padding for bf_out
    int out_padding =  (4 - (bi_out.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
     
    // remember that biWidth is in pixels and each pixel is 3 bytes  
    bi_out.biSizeImage = (((bi_out.biWidth * 3)+ out_padding)*bi_out.biHeight);
    
    // these 4 variables don't change
    bi_out.biXPelsPerMeter = bi_in.biXPelsPerMeter;
    bi_out.biYPelsPerMeter = bi_in.biXPelsPerMeter;
    bi_out.biClrUsed = bi_in.biClrUsed;
    bi_out.biClrImportant = bi_in.biClrImportant;
    
    // Create outfile's file header
    bf_out.bfType = bf_in.bfType;
    
    // These 3 variables don't change
    bf_out.bfReserved1 = bf_in.bfReserved1;
    bf_out.bfReserved2 = bf_in.bfReserved2;
    bf_out.bfOffBits = bf_in.bfOffBits; 
    
    /**
     * bfOffBits is the size of both header files (in bytes)
     * If you add that to biSizeImage (also in bytes),
     * you get the total size of the file (in bytes) 
     * Oh, happy day!!!!
     */
    bf_out.bfSize = bf_out.bfOffBits + bi_out.biSizeImage; 
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf_out, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi_out, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines -- THIS IS EACH LINE IN THE IMAGE
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


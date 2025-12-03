#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include<stdlib.h>
#include"common.h"

/* Function Definitions */

/* read the inputs from command line arguments
*validate the file extensions
*check if output file is passed or not
*if passed validate
*if not passed create
*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //read and validate the input file have .bmp or not
    if(strstr(argv[2], ".bmp")!= NULL)
    {
        printf("Input file with .bmp extension is passed\n");
         encInfo->src_image_fname =  argv[2]; 
            int flag = 0;
            for(int i =0; argv[3][i]!='\0'; i++)
            {
                if(argv[3][i] == '.') flag = i;
            }
            printf("Secret file with %s extension is passed\n", argv[3]+flag);
            encInfo->secret_fname = argv[3];
            strcpy(encInfo->extn_secret_file, argv[3]+flag);
            if(argv[4])
            {
                if(strstr(argv[4],".bmp")!=NULL){
                encInfo->stego_image_fname = argv[4];
                printf("Output file is passed\n");
                }
                else{
                    printf("Output file passed is not a .bmp file\n");
                    return e_failure;
                }
            }
            else
            {
                encInfo->stego_image_fname = "stegno_new.bmp";
                printf("Output file is created\n");
            }
            return e_success;
       /* else 
        {
            printf("Invalid secret file extension\n");
            return e_failure;
        }
            */
    }
    else
    {
        printf(".bmp file is not passed\n");
        return e_failure;
    }

}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    rewind(fptr_image);
    return width * height * 3;
}

/* Get file size
*Input : FILE *fptr_secret
*/
uint get_file_size(FILE *fptr_secret)
{
    uint size;
    fseek(fptr_secret, 0, SEEK_END);
    size = ftell(fptr_secret);
    rewind(fptr_secret);
    return size;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    printf("All files are opened without error\n");
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    uint image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    //The cover image must have enough pixels to store all header + secret bits.
    if(image_capacity >((2 + 4 + strlen(encInfo->extn_secret_file) + 4+ encInfo->size_secret_file)*8))
    {
        printf("Image file have proper capacity to encode secret data\n");
        return e_success;
    }
    else{
        printf("Image size capacity is not enough\n");
        return e_failure;
    }
}

Status do_encoding(EncodeInfo*encInfo)
{
    if(open_files(encInfo) == e_success)
    {
        if(check_capacity(encInfo) == e_success)
        {
            if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
               if((encode_magic_string(MAGIC_STRING, encInfo))  == e_success)
               {
                    int size = strlen(encInfo->extn_secret_file);
                    if(encode_secret_file_extn_size(size,encInfo) == e_success)
                    {
                        if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {

                            if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                    {
                                        fclose(encInfo->fptr_src_image);
                                         fclose(encInfo->fptr_secret);
                                         fclose(encInfo->fptr_stego_image);
                                        return e_success;
                                    }
                                    else
                                    {
                                        printf("Failure. Unable to copy remaining image data\n");
                                        return e_failure;
                                    }
                                }
                                else return e_failure; 
                            }
                            else return e_failure;
                        }
                        else  return e_failure;
                    }
                    else return e_failure;
               }
               else return e_failure;
            }
            else return e_failure;
        }
        else return e_failure;
    }
    else return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image) //dest is we pass stego_ image pointer
{
    char header[54];
    if(fread(header, 1, 54, fptr_src_image)!=54) 
    {
        printf("Can't read 54 B from source image file\n");
        return e_failure;
    }
    if(fwrite(header, 1,54, fptr_dest_image)!=54)
    {
        printf("Can't write 54 B to destination file\n");
        return e_failure;   
    }
    printf("bmp header is copied from input file to output file successfully\n");
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int i;
    for( i = 0; magic_string[i]!='\0'; i++)
    {
        char buff_bytes[8];
        //read 8 bytes from soutce file
        fread(buff_bytes, 8, 1, encInfo->fptr_src_image);
        if(encode_byte_to_lsb(magic_string[i], buff_bytes) == e_success)
        {
            fwrite(buff_bytes, 8, 1, encInfo->fptr_stego_image);
        }
        else
        {
            printf("Cant encode bytes to lsb of magic string\n");
            return e_failure;
        }
    }
    printf("Magic string is encoded in the output file successfully\n");
    return e_success;
}

Status encode_secret_file_extn_size(long extn_size, EncodeInfo*encInfo)
{
    char size_buff[32];
    fread(size_buff, 32, 1 , encInfo->fptr_src_image);
    if( encode_size_to_lsb(extn_size, size_buff) == e_success)
    {
        fwrite(size_buff, 32, 1, encInfo->fptr_stego_image);
        printf("Secret file extension size encoded successfuly\n");
        return e_success;
    }
    else
    {
        printf("Unable to encode size to lsb of secret file extension size\n");
        return e_failure;
    }
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char ext_buff[8];
    int i;
    for(i = 0; file_extn[i]!='\0'; i++)
    {
        fread(ext_buff, 8, 1, encInfo->fptr_src_image);
        if(encode_byte_to_lsb(file_extn[i], ext_buff) == e_success)
        {
            fwrite(ext_buff,  8, 1, encInfo->fptr_stego_image);
        }
        else
        {
            printf("Cant encode bytes of extension\n");
            return e_failure;
        }
    }
    printf("Secret file extension encoded successfully\n");
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char secret_size_buff[32];
    fread(secret_size_buff, 32, 1 , encInfo->fptr_src_image);
    if( encode_size_to_lsb(file_size, secret_size_buff) == e_success)
    {
        fwrite(secret_size_buff, 1, 32, encInfo->fptr_stego_image);
        printf("Secret file size successfully\n");
        return e_success;
    }
    else
    {
        printf("Unable to encode size to lsb of secret file size\n");
        return e_failure;
    }
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char secret_data[8];
    char ch;
    while(fread(&ch, 1, 1,encInfo->fptr_secret) > 0)
    {
        fread(secret_data, 8, 1, encInfo->fptr_src_image);
        if(encode_byte_to_lsb(ch, secret_data) == e_success)
        {
            fwrite(secret_data, 8, 1, encInfo->fptr_stego_image);
        }
        else
        {
            printf("Unable to encode byte to lsb of secret file data");
            return e_failure;
        }
        
    }
    printf("Secret file data encoded successfully\n");
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char rem_buff[1024];
    size_t n;
    while((n = fread(rem_buff, 1, 1024, fptr_src))>0)
    {
        fwrite(rem_buff, 1, n, fptr_dest);
    }
    printf("Remaining image data is copied to the output file successfully\n");
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int i;
    for(i = 7; i >=0; i--)
    {
        image_buffer[7-i] = (image_buffer[7-i] & 0xFE) | ((data>>i) & 01);
    }
    return e_success;
}

Status encode_size_to_lsb(long size, char*image_buffer)
{
    int i = 0;
    for(i = 31; i >= 0; i--)
    {
        image_buffer[31-i] = (image_buffer[31-i] & 0xFE) | ((size>>i) & 01);
    }
    return e_success;
}

#include <stdio.h>
#include "decode.h"
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

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    //read and validate the input file have .bmp or not
    if(strstr(argv[2], ".bmp")!= NULL)
    {
        printf("Input file with .bmp extension is passed\n");
        decInfo->stego_out_image_fname =  argv[2];
        if(argv[3]!=NULL)
        {
            char *buffer = (char *)malloc(strlen(argv[3]) + 10);
            int i = 0;
            for(i = 0; argv[3][i]!='\0'; i++)
            {
                if(argv[3][i] == '.') break;
                buffer[i] = argv[3][i];
            }
            buffer[i] = '\0';
            decInfo->output_file_fname = buffer;
            printf("Output file is passed\n");
        }
        else
        {
            decInfo->output_file_fname = "decoded_secret_msg";
            printf("Output file is created\n");
        }
        return e_success;
    }
    else
    {
        printf(".bmp file is not passed for decoding\n");
        return e_failure;
    }
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Dest Image file and Output text file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_encoded_file(DecodeInfo *decInfo)
{
    // Dest Image file
    decInfo->fptr_stego_out_image= fopen(decInfo->stego_out_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_out_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n",decInfo->stego_out_image_fname);
        return e_failure;
    }
    // No failure return e_success
    printf("File opened without error\n");
    return e_success;
}

Status open_output_file(DecodeInfo *decInfo)
{
    // Output file
    decInfo->fptr_output_file = fopen(decInfo->output_file_fname, "w");
    // Do Error handling
    if (decInfo->fptr_output_file == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_file_fname);

    	return e_failure;
    }

    // No failure return e_success
    printf("File opened without error\n");
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if(open_encoded_file(decInfo) == e_success)
    {
        fseek(decInfo->fptr_stego_out_image, 54, SEEK_SET);
        if(decode_magic_string(MAGIC_STRING, decInfo) == e_success)
        {
            if(decode_secret_file_extn_size(decInfo) == e_success)
            {
                if(decode_secret_file_extn(decInfo) == e_success)
                {
                    strcat(decInfo->output_file_fname, decInfo->secret_file_extn); 
                    if(decode_secret_file_size(decInfo) == e_success)
                    {
                        if(open_output_file(decInfo) == e_success)
                        {
                            if(decode_secret_file_data(decInfo) == e_success)
                            {
                                free(decInfo->secret_file_extn);
                                fclose(decInfo->fptr_output_file);
                                fclose(decInfo->fptr_stego_out_image);
                                return e_success;
                            }
                            else
                            {
                                return e_failure;
                            }
                        }
                        else
                        {
                            return 0;
                        }
                    }
                    else
                    {
                        return 0;
                    }
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char from_image_buffer[8] = {0};
    char *decoded_magic_string = (char *)malloc(strlen(magic_string) + 1);
    char data = 0;
    int i;
    for(i = 0; magic_string[i]!='\0'; i++)
    {
        fread(from_image_buffer, 1, 8, decInfo->fptr_stego_out_image);
        decode_byte_from_lsb(&data, from_image_buffer);
        decoded_magic_string[i] = data;
    }
    decoded_magic_string[i] = '\0';
    if(strcmp(decoded_magic_string, magic_string) == 0)
    {
        free(decoded_magic_string);
        return e_success;
    }
    else
    {
        printf("File passed is not encoded\n");
        return e_failure;
    }
}

Status decode_secret_file_extn_size(DecodeInfo*decInfo)
{
    char from_image_buffer[32] = {0};
    fread(from_image_buffer,1,32, decInfo->fptr_stego_out_image);
    if(decode_size_from_lsb(&decInfo-> secret_file_extn_size, from_image_buffer)  == e_success)
    {
        printf("Secret file extension size decoded successfully\n");
        return e_success;
    }
    else
    {
        printf("Error : unable to decode extension size\n");
        return e_failure;
    }
}
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    decInfo->secret_file_extn = malloc(decInfo->secret_file_extn_size + 1);
    char from_image_buffer[8] = {0};
    char data1 = 0;
    int i;
    for(i = 0; i < decInfo->secret_file_extn_size; i++)
    {
        fread(from_image_buffer, 1, 8, decInfo->fptr_stego_out_image);
        if(decode_byte_from_lsb(&data1, from_image_buffer) == e_success)
        {
            decInfo->secret_file_extn[i] = data1;
        }
        else
        {
            printf("Cant decode extension\n");
            return e_failure;
        }
    }
    decInfo->secret_file_extn[i] = '\0';
    printf("Secret file extension decoded successfully\n");
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char from_image_buffer[32] = {0};
    fread(from_image_buffer, 1, 32, decInfo->fptr_stego_out_image);
    if(decode_size_from_lsb(&decInfo->secret_file_size, from_image_buffer)  == e_success)
    {
        return e_success;
    }
    else
    {
        printf("Error : unable to decode file size\n");
        return e_failure;
    }
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char from_image_buffer[8] = {0};
    char data2 = 0;
    int i;
    for(i = 0; i < decInfo->secret_file_size; i++)
    {
        fread(from_image_buffer, 1, 8, decInfo->fptr_stego_out_image);
        if(decode_byte_from_lsb(&data2, from_image_buffer) == e_success)
        {
           fwrite(&data2, 1, 1, decInfo->fptr_output_file);
        }
        else
        {
            printf("Cant decode secret data\n");
            return e_failure;
        }
    }
    printf("Secret file data  decoded successfully\n");
    return e_success;
}

Status decode_byte_from_lsb(char *data, char *from_image_buffer)
{
    *data = 0;
    int i;
    for( i = 0; i < 8; i++)
    {
        *data = *data | ((from_image_buffer[i] & 01 )<< (7-i));
    }
    return e_success;
}

Status decode_size_from_lsb(long *size, char*from_image_buffer)
{
    *size = 0;
    int i;
    for( i = 0; i < 32; i++)
    {
        *size = *size | ((from_image_buffer[i] & 01 )<< (31-i));
    }
    return e_success;
}
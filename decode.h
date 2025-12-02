#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file from stego Image
 */

 typedef struct _DecodeInfo
 {
    //output image file of encoding
    char *stego_out_image_fname;
    FILE *fptr_stego_out_image;

    //to store the decoded secret data
    char *output_file_fname;
    FILE *fptr_output_file;

    //to store extension size and extension
    long secret_file_extn_size;
    char *secret_file_extn;

    //to store extension file size
    long secret_file_size;
}DecodeInfo;

 /* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_encoded_file(DecodeInfo *decInfo);

/*open output file after conactenating the extension*/
Status open_output_file(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/*Decode secret file extension size*/
Status decode_secret_file_extn_size(DecodeInfo*decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
Status decode_byte_from_lsb(char *data, char *from_image_buffer);

/*Decode size from lsb of image data array*/
Status decode_size_from_lsb(long *size, char*from_image_buffer);

#endif

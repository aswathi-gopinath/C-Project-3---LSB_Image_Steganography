#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"common.h"
#include"encode.h"
#include"decode.h"
#include"types.h"

int main(int argc, char *argv[])
{
   EncodeInfo encInfo;
   DecodeInfo decInfo;
   if(argc<3)
   {
      printf("Error : Required files are not passed\n");
      return 0;
   }
   if(check_operation_type(argv) == e_encode)
   {
      if(argc<4) 
      {
          printf("Error : Required files are not passed\n");
          return 0;
      }
      printf("Selected enocode option\n");
      //read and validate the inputs
      if( read_and_validate_encode_args(argv, &encInfo) == e_success)
      {
         //do encoding
         if(do_encoding(&encInfo) == e_success)
         {
            printf("Success\n");
            return 0;
         }
         else
         {
            printf("Failure\n");
            return 0;
         }
       }
   }
   else if(check_operation_type(argv) == e_decode)
   {
      printf("Selected Decode option\n");
      //read and validate the inputs
      if( read_and_validate_decode_args(argv, &decInfo) == e_success)
      {
         //do decoding
         if(do_decoding(&decInfo) == e_success)
         {
            printf("Success\n");
            return 0;
         }
         else
         {
            printf("Failure\n");
            return 0;
         }
       }
       return 0;
   }
   else
   {
    printf("Invalid option\n");
    return 0;
   }
  return 0;
}

OperationType check_operation_type(char *argv[])
{
   if(strcmp(argv[1],"-e") == 0) return e_encode;
   else if(strcmp(argv[1],"-d" )== 0) return e_decode;
   else return e_unsupported;
}

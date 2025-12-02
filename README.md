# C-Project-3---LSB_Image_Steganography
Features : 
1. Embed text messages into .bmp images.
2. Encode magic string for identifying steged images.
3. Check if the message fits inside the image before embedding.
4. Extract (decrypt) hidden messages from images.
5. Command-line interface for easy usage.

Requirements:
1. Input image: .bmp format.
2. Input message: Text file.
3. Output : .bmp file(optional)
4. Command-line environment

Usage :
1. for encoding - ./a.out -e input.bmp message.txt output.bmp (optional)
2. for decoding - ./a.out -d encoded.bmp output.txt (optional)
   
Notes
1. Only .bmp images are supported.
2. The program ensures that the message fits in the image.
3. Designed for educational purposes and basic steganography practice.

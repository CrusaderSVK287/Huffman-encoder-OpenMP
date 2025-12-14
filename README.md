# Huffman-encoder-OpenMP
Huffman encoder paralelization with OpenMP. Includes a decoder, but only sequential.

# Why?
Encoder with OpenMP was part of a university assignment on parallel programming. I
decided to also write a decoder for fun. Also, why not?

# Usage
Usage is simple <br>
`./huffman filepath` <br>
to encode. This produces filepath.huff file, you can decode it with <br>
`./huffman -d filepath` <br>
I also provided means to control how many threads are being used, by deafault, the maximum available threads on the CPU will be used. 
To change the number of threads, use the `-n` flag like this: <br>
`./huffman -n 4 filepath` <br>
This will run the encoder on 4 threads. The flag has no effect on decoder

# How to compile.
Just clone the repository and provided you have
- make
- g++ compiler
- openmp library
you can just run `make` command and the compiled binary should be in /bin directory


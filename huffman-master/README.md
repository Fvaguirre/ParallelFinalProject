##Usage

To run the algorithm on the supplied example data, first compile

    $ make

and then run the program:

    $ ./huffman input.txt output.txt


##Source code
The Huffman Encoding/Decoding algorithm used was based on a serial implementation of Huffman encoding (https://github.com/gyaikhom/huffman). 
The algorithm has been parallelized and optimized for running on the BG/Q. 
In the interest of evaluating only the effects of parallelization on encoding and decoding input, the writing and reading of header files has been excluded 
from this implementation and the decoding of the encoded input is based off of the huffman tree created to encode the input.  
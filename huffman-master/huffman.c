#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>


#define inputSize 65536
#define MAX_INPUT_SIZE 65536
#define MAX_CODE_SIZE 131072
#define MAX_BUFFER_SIZE 65536
#define INVALID_BIT_READ -1
#define INVALID_BIT_WRITE -1

#define FAILURE 1
#define SUCCESS 0
#define FILE_OPEN_FAIL -1
#define END_OF_FILE -1
#define MEM_ALLOC_FAIL -1

//#define BGQ 1 // when running BG/Q, comment out when running on mastiff
#ifdef BGQ 
#define printTime 0
#include<hwi/include/bqc/A2_inlines.h> 
#else 
#define printTime 1
#define GetTimeBase MPI_Wtime 
#endif

//Global variables for starting and ending times as well as for MPI variables
double encodeTime = 0;
double decodeTime = 0;
double processor_frequency = 1600000000.0;
unsigned long long start_cycles=0;
unsigned long long end_cycles=0; 

int rank;
int size;
int num_alphabets = 256;
int num_active = 0;
int *frequency = NULL;
char* input;
char* localInput;
char* output;
char* localOutput;
char* coded;
int codedPos = 0;
int codedBits = 0;
int outputPos = 0;
unsigned int original_size = 0;

typedef struct {
    int index;
    unsigned int weight;
} node_t;

node_t *nodes = NULL;
int num_nodes = 0;
int *leaf_index = NULL;
int *parent_index = NULL;

int free_index = 1;

int *stack;
int stack_top;

char buffer[MAX_BUFFER_SIZE];
int bits_in_buffer = 0;
int current_bit = 0;

int eof_input = 0;

int read_bit();
int write_bit(int bit);
int flush_buffer();
void decode_bit_stream();
int decode();
void encode_alphabet(int character);
int encode(const char* ifile);
void build_tree();
void add_leaves();
int add_node(int index, int weight);
void finalise();
void init();
void readInput();

void readInput(FILE *f) {
    int c; 
    int i = 0;
    while ((c = fgetc(f)) != EOF) {
        input[i] = c;
        i++;
        original_size++;
    }
}

void determine_frequency() {
    for (int i = 0; i < inputSize; i++) {
        frequency[(int)localInput[i]]++;
    }
}

void init() {
    frequency = (int *)
        calloc(2 * num_alphabets, sizeof(int));
    leaf_index = frequency + num_alphabets - 1;
    input = (char*) calloc(inputSize, sizeof(char));
    localInput = (char*) calloc(inputSize/size, sizeof(char));
    output = (char*) calloc(inputSize, sizeof(char));
    localOutput = (char*) calloc(inputSize/size, sizeof(char));
    coded = (char*) calloc(MAX_CODE_SIZE, sizeof(int));
}

void allocate_tree() {
    nodes = (node_t *)
        calloc(2 * num_active, sizeof(node_t));
    parent_index = (int *)
        calloc(num_active, sizeof(int));
}

void finalise() {
    free(parent_index);
    free(frequency);
    free(nodes);
    free(input);
    free(localInput);
    free(output);
    free(localOutput);
    free(coded);
}

int add_node(int index, int weight) {
    int i = num_nodes++;
    while (i > 0 && nodes[i].weight > weight) {
        memcpy(&nodes[i + 1], &nodes[i], sizeof(node_t));
        if (nodes[i].index < 0)
            ++leaf_index[-nodes[i].index];
        else
            ++parent_index[nodes[i].index];
        --i;
    }

    ++i;
    nodes[i].index = index;
    nodes[i].weight = weight;
    if (index < 0)
        leaf_index[-index] = i;
    else
        parent_index[index] = i;

    return i;
}

void add_leaves() {
    int i, freq;
    for (i = 0; i < num_alphabets; ++i) {
        freq = frequency[i];
        if (freq > 0)
            add_node(-(i + 1), freq);
    }
}

void build_tree() {
    int a, b, index;
    while (free_index < num_nodes) {
        a = free_index++;
        b = free_index++;
        index = add_node(b/2,
            nodes[a].weight + nodes[b].weight);
        parent_index[b/2] = index;
    }
}

int encode(const char* ifile) {
    FILE *fin;
    if (rank == 0) {
        if ((fin = fopen(ifile, "rb")) == NULL) {
            perror("Failed to open input file");
            return FILE_OPEN_FAIL;
        }
        readInput(fin);
        fclose(fin);
    }
    // Scatter the input to the different MPI ranks
    // MPI_Scatter(bin1, bits/size, MPI_INT, binA, bits/size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(input, inputSize/size, MPI_CHAR, localInput, inputSize/size, MPI_CHAR, 0, MPI_COMM_WORLD);
    determine_frequency();
    int c;
    for (c = 0; c < num_alphabets; ++c) {
        if (frequency[c] > 0) {
            ++num_active;
        }
    }
    stack = (int *) calloc(num_active - 1, sizeof(int));
    allocate_tree();

    add_leaves();
    build_tree();
    for (c = 0; c < inputSize/size; c++) {
        encode_alphabet((int) localInput[c]);
    }
    flush_buffer();
    free(stack);
    return 0;
}

void encode_alphabet(int character) {
    int node_index;
    stack_top = 0;
    node_index = leaf_index[character + 1];
    while (node_index < num_nodes) {
        stack[stack_top++] = node_index % 2;
        node_index = parent_index[(node_index + 1) / 2];
    }
    while (--stack_top > -1) {
        write_bit(stack[stack_top]);
    }
}

int decode() {
    codedPos = 0;
    decode_bit_stream();
    return 0;
}

void decode_bit_stream() {
    int i = 0, bit, node_index = nodes[num_nodes].index;
   // printf("rank %d: codedbits: %d\n", rank, codedBits);
    while (1) {
        bit = read_bit();
        if (bit == -1)
            break;
        node_index = nodes[node_index * 2 - bit].index;
        if (node_index < 0) {
            char c = -node_index - 1;
            localOutput[outputPos] = c;
            outputPos++;
            if (++i == inputSize/size)
                break;
            node_index = nodes[num_nodes].index;
        }
    }
    //if (rank == 0) 
        //printf("\n");
   // printf("rank %d: %d.... %d\n", rank, outputPos, inputSize/size);
}

int write_bit(int bit) {
    if (bits_in_buffer == MAX_BUFFER_SIZE << 3) {
        memcpy(coded+codedPos, buffer, MAX_BUFFER_SIZE);
        codedPos+= MAX_BUFFER_SIZE*8; 
        bits_in_buffer = 0;
        memset(buffer, 0, MAX_BUFFER_SIZE);
    }
    if (bit)
        buffer[bits_in_buffer >> 3] |=
            (0x1 << (7 - bits_in_buffer % 8));
    ++bits_in_buffer;
    codedBits++;
    return SUCCESS;
}

int flush_buffer() {
    if (bits_in_buffer) {
        memcpy(coded+codedPos, buffer, (((bits_in_buffer + 7) >> 3)));
        bits_in_buffer = 0;
        codedBits+= codedBits%8;
    }
    return 0;
}

int read_bit() {
    if (outputPos == inputSize/size) {
        return END_OF_FILE;
    }
    if (codedBits == 0) {
        return END_OF_FILE;
    }
    int bit = (coded[codedPos >> 3] >> (7 - codedPos % 8)) & 0x1;
    codedPos++;
    return bit;
}

void print_help() {
      fprintf(stderr,
          "USAGE: ./huffman <input file> <output file>\n");
}

int main(int argc, char **argv) {
    if (argc != 3) {
        print_help();
        return FAILURE;
    }
    MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    init();
    if (rank == 0) {
        start_cycles = GetTimeBase();
    }
    encode(argv[1]);
    if (rank == 0) {
        end_cycles = GetTimeBase();
        encodeTime = (end_cycles-start_cycles)/processor_frequency;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        start_cycles = GetTimeBase();
    }
    decode();
    MPI_Gather(localOutput, inputSize/size, MPI_CHAR, output, inputSize/size, MPI_CHAR, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        end_cycles = GetTimeBase();
        decodeTime = (end_cycles-start_cycles)/processor_frequency;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == 0) {
        FILE* fout = fopen(argv[2], "w");
        for (int i = 0; i < inputSize; i++) {
            if (output[i] != '\0') { // There can be extra null characters tacked onto the end of the localOutputs and I don't want to figure out a better way to get rid of them
               //printf("%c", output[i]);
               fputc(output[i], fout);
            }
        }
        printf("Encoding Time: %f\nDecoding Time: %f\n", encodeTime, decodeTime);
        fclose(fout);
    }
    
    finalise();
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return SUCCESS;
}



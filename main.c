#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

union 
{
    uint32_t data;
    uint8_t bytes[4];
} b64data;

extern uint32_t b64encode(uint32_t data);

/*
 * @brief tabela de tradução reversa base64 
*/
static const uint8_t b64_decode_alphabet[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 63, 62, 62, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 63,
    0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

/**
 * @param in: buffer para codificar
 * @param out: string base64 de buffer
 * @param len_input: tamanho de in
 * @return tamanho do pad ('=')
*/
int b64encode_buffer(uint8_t * in, uint32_t * out, int len_input)
{
    while(len_input > 2)
    {
        b64data.bytes[2] = *in++;
        b64data.bytes[1] = *in++;
        b64data.bytes[0] = *in++;
        *out++ = b64encode(b64data.data);
        len_input -= 3;
    }

    switch (len_input)
    {
    case 2:
        b64data.bytes[2] = *in++;
        b64data.bytes[1] = *in++;
        *out = b64encode(b64data.data);
        ((uint8_t *)out)[3] = '=';
        break;
    case 1:
        b64data.bytes[2] = *in++;
        *out = b64encode(b64data.data);
        ((uint8_t *)out)[2] = '=';
        ((uint8_t *)out)[3] = '=';
        break;
    default:
        break;
    }
}

/**
 * @param in: string base64
 * @param out: buffer decodificado da string base64
 * @param len_input: tamanho de in
 * @return tamanho do pad ('=')
*/
int b64decode_buffer(uint8_t * in, uint8_t * out, int len_input)
{
    uint8_t a,b,c,d;

    for (int n = (len_input - 1) / 4; n>0 ; n--)
    {
        a = b64_decode_alphabet[*in++];
        b = b64_decode_alphabet[*in++];
        c = b64_decode_alphabet[*in++];
        d = b64_decode_alphabet[*in++];
        *out++ = (a << 2) + (b >> 4);
        *out++ = (b << 4) + (c >> 2);
        *out++ = (c << 6) + d;
    }

    a = b64_decode_alphabet[*in++];
    b = b64_decode_alphabet[*in++];
    *out++ = (a << 2) + (b >> 4);

    int pad = 2;

    if (*in != '=')
    {
        pad--;
        c = b64_decode_alphabet[*in++];
        *out++ = (b << 4) + (c >> 2);
        if (*in != '='){
            pad--;
            d = b64_decode_alphabet[*in++];
            *out++ = (c << 6) + d;
        }
        else
            *out++ = c << 6;
    }
    else 
        *out = b << 4;

    return pad;
}

/**
 * Verifica se a string não tem caracter que não é base64
 * @param in: string base64
 * @param size: tamanho de in
 * @return 0 se não possui, 1 se possui;
*/
int verify_b64(uint8_t * in, int size){
    while(size--)
    {
        if(b64_decode_alphabet[*in] || (*in == 'A') || (*in == '='))
            continue;
        else
            return 1;
    }
    return 0;
}

/**
 * Codifica arquivo binário em base64
 * @param input_name: nome do arquivo binário de entrada
 * @param output_name: nome do arquivo de saída
 * @return 1 se houve algum erro, se não retorna 0  
*/
int b64encode_file(const char * input_name, const char * output_name)
{
    struct stat sb; // dados do arquivo

    int input_size;
    int input_file, output_file;
    char * nline = "\r\n";

    // bloco múltiplo de 3
    uint8_t * file_data = (uint8_t *)malloc(111);
    // 2 blocos de tamanho 74
    uint32_t * b64_str  = (uint32_t *)malloc(37*sizeof(uint32_t));
    
    input_file = open(input_name, 0, 0644);
    output_file = open(output_name,  O_WRONLY | O_TRUNC | O_CREAT, 0644);

    if(input_file > 0)
    {
        fstat(input_file, &sb);
        input_size = sb.st_size;
    }
    else return 1;

    // Le blocos de 111 e escreve blocos de 148
    while(input_size > 110)
    {
        read(input_file, file_data, 111);
        b64encode_buffer(file_data, b64_str, 111);

        write(output_file, (uint8_t *)b64_str, 74);
        write(output_file, nline, 2);
        write(output_file, (uint8_t *)b64_str + 74, 74);
        write(output_file, nline, 2);

        input_size -= 111;
    }

    // Le ultimo bloco
    if(input_size){
        read(input_file, file_data, input_size);
        b64encode_buffer(file_data, b64_str, input_size);
        write(output_file, (uint8_t *)b64_str, (input_size+2)/3 * 4);
    }

    free(file_data);
    free(b64_str);
    close(input_file);
    close(output_file);

    return 0;
}

/**
 * Reverte codificação em base64
 * @param input_name: nome do arquivo base64 de entrada
 * @param output_name: nome do arquivo binário de saída
 * @return 1 se houve algum erro, se não retorna 0  
*/
int b64decode_file(const char * input_name, const char * output_name)
{
    struct stat sb; // dados do arquivo

    int input_size;
    int input_file, output_file;
    char nline_trash[2];
    
    input_file = open(input_name, 0, 0644);
    output_file = open(output_name,  O_WRONLY | O_TRUNC | O_CREAT, 0644);


    if(input_file > 0)
    {
        fstat(input_file, &sb);
        input_size = sb.st_size;
    }
    else{
        close(input_file);
        close(output_file);
        return 1;
    }
    
    // bloco múltiplo de 74
    uint8_t * file_data = (uint8_t *)malloc(148);
    // bloco resultante
    uint8_t * b64_buffer = (uint8_t *)malloc(111);

    int pad;

    // Le blocos de 148 e escreve blocos de 111
    while(input_size > 148)
    {
        read(input_file, file_data, 74);
        read(input_file, nline_trash, 2);
        read(input_file, file_data+74, 74);
        read(input_file, nline_trash, 2);
        if (verify_b64(file_data, 148))
        {
            printf("Caracter não Base64!\n");
            free(file_data);
            free(b64_buffer);
            close(input_file);
            close(output_file);
            return 1;
        }
        b64decode_buffer(file_data, b64_buffer, 37*sizeof(uint32_t));
        write(output_file, (uint8_t *)b64_buffer, 111);
        
        input_size -= 152;
    }

    // Le e escrece ultimo bloco
    if(input_size){
        read(input_file, file_data, input_size);
        if (verify_b64(file_data, input_size)){
            printf("Caracter não Base64!\n");
            free(file_data);
            free(b64_buffer);
            close(input_file);
            close(output_file);
            return 0;
        }
        pad = b64decode_buffer(file_data, b64_buffer, input_size);
        write(output_file, b64_buffer, (input_size + 3) / 4 * 3 - pad);
    }
    
    free(file_data);
    free(b64_buffer);
    close(input_file);
    close(output_file);

    return 0;
}


int main(int argc, char * argv[]){

    if(argc == 4)
    {
        if(!strcmp(argv[1], "--encode"))
        {
            if (b64encode_file(argv[2], argv[3]))
                printf("Erro ao Ler o arquivo!\n");
        }

        else if(!strcmp(argv[1], "--decode"))
        {
            if (b64decode_file(argv[2], argv[3]))
                printf("Erro ao Ler o arquivo!\n");
        }
        else
        {
            printf("Comando invalido!\n");
        }
    }
    return 0;
}


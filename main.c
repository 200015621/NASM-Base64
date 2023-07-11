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

int b64encode_file(const char * input_name, const char * output_name)
{
    int input_file, output_file;
    struct stat sb;
    
    input_file = open(input_name, 0, 0644);
    output_file = open(output_name,  O_WRONLY | O_TRUNC | O_CREAT, 0644);

    uint8_t * file_data;
    uint32_t * b64_str;

    if(input_file > 0)
    {
        fstat(input_file, &sb);

        file_data = (uint8_t *)malloc(sb.st_size);
        read(input_file, file_data, sb.st_size);
    }
    else return 0;
    
    b64_str = (uint32_t *)malloc((sb.st_size + 2) / 3 * 4);

    uint8_t * in = file_data;
    uint32_t * out = b64_str;

    for (int n = sb.st_size / 3; n > 0; n--)
    {
        b64data.bytes[2] = in[0];
        b64data.bytes[1] = in[1];
        b64data.bytes[0] = in[2];
        *out++ = b64encode(b64data.data);
        in += 3;
    }

    switch (sb.st_size % 3)
    {
    case 2:
        b64data.bytes[2] = in[0];
        b64data.bytes[1] = in[1];
        *out = b64encode(b64data.data);
        ((uint8_t *)out)[3] = '=';
        break;
    case 1:
        b64data.bytes[2] = in[0];
        *out = b64encode(b64data.data);
        ((uint8_t *)out)[2] = '=';
        ((uint8_t *)out)[3] = '=';
        break;
    default:
        break;
    }

    /* Print Temporario */

    
    int i;
    size_t buf_size = (sb.st_size + 2) / 3 * 4;
    char * nline = "\r\n";
    in = (uint8_t *)b64_str;
    for (i = buf_size; i>73; i-=74)
    {
        write(output_file, in, 74);
        write(output_file, nline, 2);
        in += 74;
    }
    write(output_file, in, buf_size%74);

    free(file_data);
    free(b64_str);
    close(input_file);

    return 1;
}

static const uint8_t b64_decode_alphabet[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 63, 62, 62, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 63,
    0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

int remove_non_b64(uint8_t * in, uint8_t * out){

    int size = 0;
    while(*in)
    {
        if(b64_decode_alphabet[*in] || (*in == 'A') || (*in == '=')){
            *out++ = *in++;
            size++;
        }
        else
            in++;
    }
    *out = 0;

    return size;
}

int b64decode_file(const char * input_name, const char * output_name)
{
    int input_file, output_file;
    struct stat sb;
    int size;
    
    input_file = open(input_name, 0, 0644);
    output_file = open(output_name,  O_WRONLY | O_TRUNC | O_CREAT, 0644);

    uint8_t * file_data;
    uint8_t * b64_str;
    uint8_t * b64_buf;

    if(input_file > 0)
    {
        
        fstat(input_file, &sb);
        b64_str = (uint8_t *)malloc(sb.st_size + 1);
        b64_buf = (uint8_t *)malloc(sb.st_size + 1);
        read(input_file, b64_buf, sb.st_size);
        b64_buf[sb.st_size] = 0;

        /* remove caracteres que nao sao base64 */
        size = remove_non_b64(b64_buf, b64_str);
        file_data = (uint8_t *)malloc((size + 3) / 4 * 3);

    }
    else return 0;


    uint8_t *in = b64_str;

    uint8_t *out = (uint8_t *)file_data;
    uint8_t a,b,c,d;

    for (int n = (sb.st_size - 1) / 4; n>0 ; n--)
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
        *out++ = b << 4;
    
    write(output_file, file_data, (size + 3) / 4 * 3 - pad);

    
    free(file_data);
    free(b64_str);
    free(b64_buf);
    close(input_file);
    close(output_file);

    return 1;
}


int main(int argc, char * argv[]){

    if(argc == 4)
    {
        if(!strcmp(argv[1], "--encode"))
        {
            if (!b64encode_file(argv[2], argv[3]))
                printf("Arquivo nao existe!\n");
        }

        else if(!strcmp(argv[1], "--decode"))
        {
            if (!b64decode_file(argv[2], argv[3]))
                printf("Arquivo nao existe!\n");
        }
        else
        {
            printf("Comando invalido!\n");
        }
    }
    return 0;
}


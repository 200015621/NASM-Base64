# Codificador Base64 em C NASM
Software Básico - Ladeira

## Windows

### Compilar
```
nasm -f win64 base64.asm
gcc -g main.c base64.obj -o main.exe
```

### Rodar

```
./main.exe --encode <input_file> <output_file>
./main.exe --decode <input_file> <output_file>
```

## Linux

### Compilar
```bash
nasm -f elf64 base64.asm
gcc -g main.c base64.o -o main
```

### Rodar

```
./main --encode <input_file> <output_file>
./main --decode <input_file> <output_file>
```

# Codificador Base64 em C NASM
Software Básico - Ladeira

# Windows

### Compilar
```bash
nasm -f win32 base64.asm
gcc -g -m32 main.c base64.obj -o main.exe
```

### Rodar

```
./main.exe --encode <input_file> <output_file>
./main.exe --decode <input_file> <output_file>
```

# Linux

### Compilar
```bash
nasm -f elf base64.asm
gcc -g -m32 main.c base64.o -o main
```

### Rodar

```
./main --encode <input_file> <output_file>
./main --decode <input_file> <output_file>
```

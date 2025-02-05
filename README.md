# ***spud*** - ***S***imple ***P***rocessor ***U***nit ***D***esign
The simplest processor design (that I could think of) that still resembles modern processors:
 - Von Neumann architecture.
 - Accumulator based machine.
 - Word (16-bits) addressable memory.
### Contents
- [Instruction Set](#instruction-set)
- [Machine Code Syntax](#machine-code-syntax)
- [Assembly Code Syntax](#assembly-code-syntax)
- [Assembler](#assembler)
- [Simulator](#simulator)
- [Peripherals](#peripherals)
- [Assembly Code Examples](#assembly-code-examples)
# Instruction Set
```
______________________________________
|            m e m o r y             | 
|____________________________________|
             |          |
  _____ADDRESS          DATA__________________
  |          |          |                    | ___NOT (~)
  |   ______ | ________ | _____________     _|_|_
  |   |      |   |      |   |         |     \xor/ 
__|___|__  __|___|__  __|___|__     __|__   __|__
|  i p  |  |  i r  |  |  a c  |  ___\    \_/    /__CARRY (+1)
|_______|  |_______|  |_______|  |   \__adder__/  
    |          |          | |____|        |   
    |__________|__________|_______________|

(ip) instruction pointer 13-bits

(ir) instruction register 16-bits
     opcode  bits 13-15 
     address bits  0-12

(ac) accumulator 17-bits
     sign_bit  bit 15
     carry_bit bit 16
```
|Fetch/Decode Behaviour |
|-----------------------|
|```ir = memory[ip++]```|

|Execute Behaviour                                |Operation                |
|-------------------------------------------------|-------------------------|
|```if(opcode == 0) {ac = memory[address]}```     |```ld``` - Load          |
|```if(opcode == 1) {memory[address] = ac}```     |```st``` - Store         |
|```if(opcode == 2) {ac += memory[address]}```    |```ad``` - Add           |
|```if(opcode == 3) {ac = ~memory[address]}```    |```nt``` - Not           |
|```if(opcode == 4) {ip = address}```             |```ja``` - Jump Address  |
|```if(opcode == 5 && sign_bit) {ip = address}``` |```js``` - Jump Sign Bit |
|```if(opcode == 6 && carry_bit) {ip = address}```|```jc``` - Jump Carry Bit|
# Machine Code Syntax
[Backus-Naur form](https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form)
```
<bit> ::= "0" | "1"
<nibble> ::= <bit><bit><bit><bit>
<word> ::= <nibble><nibble><nibble><nibble>
<opcode> ::= "000" | "001" | "010" | "011" | "100" | "101" | "110"
<address> ::= <bit><nibble><nibble><nibble>
<code> ::= <opcode><address><code> | <word><code> | ""     
```
# Assembly Code Syntax
[Backus-Naur form](https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form)
```
<digit> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
<number> ::= <digit><number> | <digit>
<letter> ::= "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z" | "_"
<label> ::= <upper><label> | <upper>
<whitespace> ::= "\n" | "\s" | "\t"
<space> ::= <whitespace><space> | <whitespace>
<opcode> ::= "ld" | "st" | "ad" | "nt" | "ja" | "js" | "jc"
<operation> ::= <opcode><space><number> | <opcode><space><label>
<code> ::= <label><space><code> | <number><space><code> | <operation><space><code> | ""

```
# Assembler
The assembler takes two filepaths as arguments:  
```python asm.py [INPUT_ASSEMBLY_FILEPATH] [OUTPUT_BINARY_FILEPATH]```  

For example if you wanted to regenerate the hello world assembly example:  
```python asm.py examples/asm/hello_world.asm examples/bin/hello_world.bin```

If any errors occur try running the Makefile.  
# Simulator
The simulator will run the examples/bin/hello.bin if no arguments are given:  
```./sim.bin```
  
Terminal Output:  
```
hello
$
```
If any errors occur try running the Makefile

The simulator supports the following optional arguments:
- ```-filepath [PATH_TO_BINARY]``` - Run the .bin at the given filepath.
- ```-cycle_time [NUMBER_MILLISECONDS]``` - Make the simulator take the given number of milliseconds per cycle.
- ```-cycle_count [NUMBER_CYCLES]``` - Make the simulator run the given number of cycles before halting.

For example if you wanted to execute hello.bin 4x faster and have it stop before the newline is printed:  
```./sim.bin -filepath examples/bin/hello_world.bin -cycle_time 25 -cycle_count 50```
# Peripherals
There are two memory mapped peripherals:
- Input Unit, address ```8190``` (assembly label ```GETC```)
- Output Unit, address ```8191``` (assembly label ```PUTC```)

These peripherals are used to read / print characters to standard in/out (terminal).

***Input Unit***: set memory address ```8190``` to zero value, memory address ```8190``` will then be set to ascii value of the next character from stdin.

***Output Unit***: set memory address ```8191``` to ascii value of next character for stdout, memory address ```8191``` will then be set to zero value.

See ```example_asm/echo.asm``` for an example of how to use the input and output units.
# Assembly Code Examples
---
```
int A = 7;
int B = 2;
int C = A - B;
```
The processor lacks subtraction hardware.
The workaround is to negate one of the numbers then add them together.
Negate the number using ones's complement.
Bitwise not the number, after adding the numbers together add one if there was a carry out.
```
START nt B
      ad A
      jc CARRY
      ja STORE
CARRY ad ONE
STORE st C
HALT  ja HALT

ONE   1

A     7

B     2

C     0
```
---
```
int A = 7;
int B = 2;
int C = A - B;
```
The processor lacks subtraction hardware.
The workaround is to negate one of the numbers then add them together.
Negate the number using two's complement.
Bitwise not the number, then add one to the number.
```
START nt B
      ad ONE
      ad A
      st C
HALT  ja HALT

ONE   1

A     7

B     2

C     0
```
---
```
int A[4] = {1, 2, 4, 8};
int I = 2;
int J = A[I];
```
All instructions have fixed memory addresses.
Dynamic memory addresses requires self-modifying code.
Create and load "ld" instruction with array index address (INDEX).
```
START ld A_LD
      ad I
      st INDEX
INDEX 0
      st J
HALT  ja HALT

I     2

J     0

A_LD  ld A
A     1
      2
      4
      8
```
---
```
int A[4] = {1, 2, 4, 8};
int I = 2;
int J = A[I] = J;
```
All instructions have fixed memory addresses.
Dynamic memory addresses requires self-modifying code.
Create and store "st" instruction with array index address (INDEX).
```
START ld A_ST
      ad I
      st INDEX
      ld J
INDEX 0
HALT  ja HALT

I     2

J     0

A_ST  st A
A     1
      2
      4
      8
```
---
```
printf("hello\n");
```
All instructions have fixed memory addresses.
Dynamic memory addresses requires self-modifying code.
Pass "ja" instruction with return address (HALT) to subroutine (PRINT) and store it (PRINT_RET) for later execution to return. 
```
           ld  STRING
           st  PRINT_ARG
           ld  HALT
           ja  PRINT_CAL
HALT       ja  HALT


PRINT_ARG  0
PRINT_CAL  st  PRINT_RET
           ld  PRINT_ARG
           st  PRINT_IDX
PRINT_IDX  0
           st  PUTC
           st  PRINT_ARG
           ld  PRINT_IDX
           ad  INCREMENT
           st  PRINT_IDX
           nt  PRINT_ARG
           ad  INCREMENT
           ad  CHAR_NULL
           js  PRINT_IDX
PRINT_RET  0


INCREMENT  1


STRING     ld  CHAR_ARRAY
CHAR_ARRAY 104
           101
           108
           108
           111
           10
CHAR_NULL  0
```
---

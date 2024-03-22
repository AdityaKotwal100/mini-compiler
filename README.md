# Mini Compiler
## Overview

This project involves the development of a simple compiler aimed at providing a practical understanding of compiler construction principles. The compiler is designed for a simple language specified by a given grammar. Its core functionality includes parsing input programs, generating an intermediate representation (IR), and executing the IR using a provided interpreter. The focus is on educational insights into compiler design rather than production-level application.

## Features

- **Grammar-Based Parsing**: Implements a parser that adheres to a predefined grammar, ensuring accurate syntactical analysis of the input program.
- **Intermediate Representation (IR) Generation**: Transforms parsed input into an IR, represented as a linked list of instructions, facilitating subsequent execution phases.
- **Execution via Interpretation**: Utilizes a provided interpreter to execute the IR, enabling the evaluation of the program's logic without the need for assembly code generation.
- **Support for Key Programming Constructs**: Handles essential programming constructs, including variable declarations, assignment, conditional (if, while), iterative (for), and selection (switch) statements, as well as input/output operations.

## Technical Details

- **Language**: The project is implemented in C++, taking advantage of its features for efficient data structure manipulation and control flow management.
- **Input Program Structure**: Expected to conform to the specified grammar, encompassing a `var` section for variable declarations followed by a `body` containing executable statements.
- **IR Execution**: The execution process interprets the IR to simulate the runtime behavior of the input program, with support for basic arithmetic operations and control flow constructs.

## Usage

To utilize this compiler, users are expected to have a basic C++ development environment. Compilation and execution instructions are as follows:

1. Compile the compiler source code using a C++ compiler (e.g., g++).
2. Execute the compiled binary, passing the path to the input program file as an argument.


**Sample Input 1:**
```i, j;
{
	input i;
  	i = 42 ;
  	j = i + 1; 
	output i;
	output j;
}
1 2 3
```

**Expected Output 1:**
```
42 43
```

**Sample Input 2:**
```
a  ;
{
input a;

WHILE  a > 0 {
	output a;
	a = a-1;
}
}
10 2 3 4
```

**Expected Output 2:**
```
10 9 8 7 6 5 4 3 2 1 
```

**Sample Input 3:**
```

j, i, minusfour, k ;

{
i = 4;
j = 3;

minusfour = 0-4;


WHILE i > 0 {
	j = 3;		
	WHILE j > 0 {
		k = i*j;
		output k;
		j = j-1;
	}
	i = i-1;
}

WHILE i > minusfour {
	output i;
	i = i - 1;
}

i = i+1;
j = j+1;
output i;
output j;

}
9 8 3 7

```

**Sample Output 3**
```
12 8 4 9 6 3 6 4 2 3 2 1 0 -1 -2 -3 -3 1 
```

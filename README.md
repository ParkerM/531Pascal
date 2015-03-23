# CSCE 531 Super Project

## Pascal Compiler Over 3 Iterative Projects

#### Project 1:

Process Pascal global type and variable declarations. This involves both installing the declarations into the symbol table and allocating memory for the variables in the assembly language output file. Also, after all declarations have been processed, you should dump the symbol table (using st_dump() from symtab.h); to do this, change the `#if 0` directive in the `main()` function to `#if 1`.

Your compiler should read Pascal source code from `stdin` and write the x86 assembly language output to `stdout`. Your compiler executable should be called `ppc3`. You will not have to emit assembly code explicitly, but rather call appropriate routines in the back end (`backend-x86.c` and `backend-x86.h`). Besides altering the `gram.y` file, put syntax tree-building functions into a new file `tree.c`, with definitions for export in `tree.h`. Put code-generating routines into a new file `encode.c`, with definitions for export in `encode.h`. With few exceptions throughout the project, all backend routines are called from `encode.c` (some may be called directly from the grammar). No backend routines should be called from `tree.c`, hence you will not need to include `backend-x86.h` in `tree.c`.

**To receive 80% of the credit**: You must be able to process the following basic Pascal type specifiers: Boolean, Integer, Char, Single, and Real. The C equivalents of these types are char, long, unsigned char, float, and double, respectively. You must also be able to handle pointer and array type constructors. You may limit the syntax so that array dimensions must always be given as subranges with unsigned integer constants as limits. A symbol table entry should be made for each id, whether a type name or a variable name. The entry should indicate the type of the declaration. You must be able to resolve forward type references, as described in class. Routines for building and analyzing types are in the types module (types.h), and routines for manipulating the symbol table are in the symbol table module (symtab.h). You are required to use these modules, but you are not allowed to modify them. For more on these and the other modules, see the Resources section, below.

**To receive 90% of the credit**: In addition to obtaining the 80% level, you should also allow procedure and function type modifiers (with no parameters). In Pascal, functions are like functions in C; that is, they return a value of some specified type, and a call to a function is treated as an expression of that return type. Procedures are just like C functions with void return type, except that procedure calls are treated as statements rather than expressions (how these are called is not relevant for this project installement, though). You should add the necessary semantic checks and error messages to support function modifiers (it is illegal for a function to return a function, for example). You should assume that functions can return only "simple" types, i.e., the basic types listed above and pointer types.

**To receive 100% of the credit**: In addition to obtaining the 90% level, you should allow parameters to functions and procedures. Pascal allows two types of parameters: value parameters (for call-by-value, same as in C) and var parameters (for call-by-reference, similar to '&' parameters in C++). Var parameters are those in a list of parameter names following the var keyword. In this stage of the project, you only need to flag which kind a parameter is, otherwise you won't treat them differently.

The x86 (actually 32-bit i386) assembly code to be emitted for this assignment is generated automatically by calling functions in backend-x86.c, which I discuss briefly in class.

At all levels you are responsible for detecting duplicate declarations. At the 100% level, you must also detect duplicate declarations in parameter lists.

Your compiler should be capable of detecting multiple semantic errors in one file. You can make arbitrary decisions about how to proceed when errors occur (for instance, with a duplicate declaration you might decide to ignore the second declaration). The important point is to do something so you can proceed (without causing a later bus error, segmentation fault, etc. during compilation).

You may allow the compiler to stop processing with the first syntax error. A syntax error is defined with respect to the distributed Pascal grammar (gram.y, see next paragraph).

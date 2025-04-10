# RIPL VM: RPN-based Interpretive Programming Language Virtual Machine

## Intro

This is a small demo project to demonstrate what goes into a Virtual Machine. I am not doing extensive error checking as I want to highlight the concepts and not drown out the code in error checking code.

If you are interested in this project I highly encourage you to take a look at my other related project named RPN-Script [here](https://github.com/quasar-pankaj/rpnscript).

### RPN Syntax

RPN or Reverse Polish Notation is an alternative syntax to the usual Infix notation that we normally use. What we would write as a + b in Infix would be a b + in RPN syntax. Each token is delemited by a white space (space, tab, or newline), so white spaces play a vital role in RPN.

### Why RPN?

Most books on compiler construction are filled with very abstract mathematical notations which make understanding the theory very difficult for many including myself. So, I wanted to show that it is not really a difficult subject and can be fun if you get the hang of it. I wanted to demonstrate some techniques and chose RPN because it was unusual but not unique because languages like Forth have used RPN before this. This language is basically intendd as a toy language to be used for instructional purposes to demonstrate techniques other than parsing (parsing itself is a fun subject in its own right).

### How this project is structured

This project is divided into 4 distinct sub projects:- 1. **dism** which is a disassembler for the .bc files. I used it extensively when developing the compiler and VM. 2. **libripl** is a collection of code that is used by all the other 3 projects. 3. **ripl** this is the virtual machine that executes the .bc files. bc simply stands for byte code. 4. **riplc** is the compiler for the virtual machine.

### Demos

We will start with a simple Hello World program:

#### Hello World

"Hello World!" = end

A string ("Hello World!") is pushed on to the stack, = prints out the top element of the stack and end is simply that the end of the program. Now execute riplc to compile the program ./riplc helloworld.rpn. This will produce helloworld.rpn.bc. Execute the compiled bytecode using ./ripl helloworld.rpn.bc you will get the following output:-
Hello World!

#### Misc. Operators

10 30 3 / + =
10 3 / =
5 5 >= 3 5 >= && =
end

Lets understand what the program is doing.
Line 1: 10 30 3 / + = in infix form means 30/3 + 10 the output would be 20
Line 2: 10 3 / = the output would be 3.3333
Line 3: 5 5 >= 3 5 >= && = is RPN for (5>=5 && 3>=5). The result is false.

#### Other scripts

There are many more scripts in the scripts folder. You can play with them and see how they work.

## Outro

Please let me know what you think of the project.

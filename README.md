# Variadic Visitor

## Description

This is a simple example of a variadic visitor pattern. The main concept is for when we have a templated function which takes a number of templated arguments, and we want an easy way of calling that function with types deduced at runtime. This is an alternative to using the visitor pattern, which can acheive the same thing, however I wanted to see if I could do it only using variadic templates. 

## Usage

To build and run the program, simply run the following commands:

```bash
mkdir build
cd build
cmake ..
./variadic
```

The program will print out the types of the arguments passed to the function, and then print out the types of the arguments passed to the next function in the chain.
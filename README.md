# Variadic Visitor

## Description

This is a simple example of a variadic visitor pattern. The main concept is for when we have a templated function which takes a number of templated arguments, and we want an easy way of calling that function with types deduced at runtime. This is an alternative to using the visitor pattern, which can acheive the same thing, however I wanted to see if I could do it only using variadic templates. 

## Background

This small project originated from a problem I encountered while working on another project. I had a function which took three templated arguments, each belonging to a different 'class' of types (using concepts), and I wanted to call that function with types deduced at runtime. With only one template argument, this is simple enough, just use a switch statement to call the appropriate function. However, with multiple template arguments, it gets a bit more complicated.

A straightforward way to go about this is to use a factory:
```c++
    Parent* get_type(std::string_view type);
```
This is known as *dynamic polymorphism*, and is very common in C++, but it comes with a few drawbacks. Firstly, it is well known that there is a performance overhead associated with the indirection caused by use of *V Tables*. In my project, this particular function would then call a performance critical bit of code in a hot loop, and would have to deal with this branching and indirection every iteration (compiler magic notwithstanding) if a virtual-based solution was to be used. Surely C++ has a (rather verbose) solution for this.

Every time we need to make decisions at runtime, there is going to be some branching involved where the control flow needs to find the right path to execute. However, by switching out dynamic polymorphism for the *visitor pattern* we can "up-front" all of this indirection when we are setting thing up and initialising, and then once we are in the correct function overload, the types are already determined and there isn't any more indirection. Here is an example using std::variant and std::visit:

```c++
    using v_InputHandler = std::variant<FileReader, StreamReader>;
    using v_OutputHandler = std::variant<FileWriter, StreamWriter>;

    v_InputHandler v_input(FileReader{});
    v_OutputHandler v_output(StreamWriter{});

    std::visit([](auto& reader, auto& writer){
        while (b_app_open) {
            writer << reader.process();
            writer.process();
        }
    }, v_input, v_output);
```

So this is pretty great. Since we only have a few possible types each inputs and outputs, and they are known at compile time, creating a virtual base class for each, leaving the runtime to consult the V-Table to find the correct function override for each reader and writer on every iteration indeed seems a bit overkill. Moreover, this strategy would result in pretty *inexpressive* code: by looking only at the function, how can one tell what counts as an 'Input' or 'Output' type? This visitor implementation handles both of those concerns by (in the first case) moving our loop inside the function overload so that the branching happens only once and (in the second) it is plainly obvious which types this call is supposed to serve, since they are held in another type called a variant. 

Perfect, so why did make my own? 

Well, i'm afraid it's a case of not thinking broadly enough when problem solving. In my head, I had function which needed to be called with types known at compile time, but as these types depended on runtime parameters, there needed to be some branching happening based on these parameters. As you cannot have a function whose return type varies, any factory implementation was out of the question unless we wanted to upcast and go down the virtual route which, as mentioned, is not the right tool for this job. So, my brain happily forgot all about std::variant and jumped straight to "I wonder if this can be done with variadic templates". Hence, this project. It basically does exactly the same thing as std::visit - it leverages C++ lambdas as a way to pick the right function overload at runtime - but it is infinitely less flexible. So, you should probably not use this library.

Sometimes it can be good to go over the fundamentals. Otherwise, it is too easy to take the nice things we have for granted, and stop looking for new, better ways to do things. For me (someone who is looking to improve my skills), it has been a good way to motivate myself and gain some confidence that I do know what I'm doing, even if I forget things from time to time.

## Usage

Tested on G++-14 only.

To build and run the program, simply run the following commands:

```bash
mkdir build
cd build
cmake ..
./variadic
```

The program will print out the types of the arguments passed to the function, and then print out the types of the arguments passed to the next function in the chain.
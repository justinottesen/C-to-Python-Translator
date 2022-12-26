C to Python Translator by Justin Ottesen

I know this is almost entirely useless, and going from Python to C would be
far better, but going this direction is easier to do because C gives more
information and python is more forigiving. I am using this to explore
compilers/interpreters on my own before I start learning from a real source.

This will likely contain some horribly inefficient methods, and will not work
for the majority of C programs, but will be a fun project.

RESTRICTIONS ON C PROGRAM:

1. Must only be one file.
2. Each line must be <= 256 chars
3. No command line arguments.
4. No structs.
5. Passing by value vs by reference will be weird
6. Sophistocated print statements may be weird.
7. Very picky with spacing, and very bad error messages.
8. No Pointers (except char* as string, must be [char*] not [char *])
9. Ignores preprocessor directives.

Again, this is almost useless, but was a fun little challenge. I really
recommend against using this for anything other than inspiration for an idea.

Found C to be pretty limiting, working on a similar project in C++ that will
function as a C deubugger. This is far more ambitious, but reading a book as
a guide. The book was written before C++98 though, so incredibly outdated.
It talks about the "new and exciting language of Java". Honestly a good thing,
keeps me from just copying code directly, and makes me really understand it. 
# Farmlang
Farmlang is an animal themed esolang (esoteric language), made to de a comic, chaotic programming language. Detailed information (syntax, rules, etc) is on README.

## Usage
Farmlang code can be written at .farm files using text editors, and interpreted using the farmlang.c file (which needs the farmlang.h header). Usage must be: "./farmlang file.farm" on an terminal, with farmlang being the compiled version of farmlang.c.

### Functions
barn {} - Main function, needed to everything else. Must be used in the beginnning of every farmlang code.
pig - Declares a string variable - (pig var = string) or (pig var = "string"). Has a 5% chance of declaring "oink" instead of the original string.
cat - Declares a int variable, but using cat onomatophoeias instead of actual numbers (meow = 1, purr = 5, hiss = 10). Onomatophoeias can be used together either with space inbetween them to sum their values ("cat var = meow purr" makes the variable 6) or to multiply their values, firstly adding the consecutive meows and them multiplying with the rest ("cat var = purrhiss"  makes var = 50 and "cat var = meowmeowmeowpurr" make var = 15).
cow() - Prints a variable, either a number or a string.
chicken - Declares a variable based on user input ("chicken var" makes var = whatever user types in)
horse(var, num1, num2, operator) - Does a math operation specified by the operator (either "+" for sum, "-" for subtraction, "*" for multiplication and "/" for division") between num1 and num2, and puts the result inside the var variable.
owl(){} - Used as a if operator. If condition inside parenthesis () is true, executes the code inside {}

#### Rules
As Farmlang is a limited language, it does not correctly recognize spaces and break lines at some situations (for example, spaces between functions and their parenthesis, like "cow ()" instead of correct "cow()" and brackets in different line of their function, like "barn\n{" instead of "barn{")
When executed,

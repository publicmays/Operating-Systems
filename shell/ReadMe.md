Operating-System TA's please read the ReadMe.md in better format here at this link :
https://github.com/katlew3/Operating-Systems/tree/master/shell#configuration

```
execvp
```
- is allowed stated by Sumi Helal.

#### Configuration
_Install flex and bison_
```
sudo apt-get install flex
sudo apt-get install bison
```
> flex installs lex
> bison installs yacc

#### Implemented

* To run the commands from an input file in our shell
* Inside our shell run the command
```
./shell < file.txt
```

do not do this
```
./shell<file.txt
```
* all of the output will be in STDOUT which is the shell

WE DO NOT RECOMMEND THIS

```
./shell < file.txt > output.txt
```
* If you redirect to an output file only the non-built in commands will display
* Results in output.txt will be limited, and we only suggest comparing the results to STDOUT not the output.txt


Built-in Commands

* Environment Variables
 * setenv
 * printenv
 * unsetenv

* Alias
 * alias (with args)
 * alias (without args)
 * unalias
 * Nested aliases allowed, infinite alias expansion is handled

* Others
 * cd (with no args, with cd ~, with directory name)
 * bye


Other Commands
* Standard shell commands such as:
```
echo : ls : wc
```
 * The following commands are handled appropriately if the user inputs no args:
```
cat : sort : cksum : fold : head : nl : od : tail : tee : wc
```
* I/O and error redirection, as well as background processing through:
```
> : >> : < : 2> : 2>&1 : &
```
* Pipelining, for example:
```
echo hello world | ls
```
Environment Variable Expansion
* Expands ${variable}

Wildcard Matching
* Expands both * and ?
```
wc *.c > out.txt
```
* Execvp handles '.' and '/'

Tilde Expansion
* Tidle expansion is working.


#### Special Commands
```
2>&1
```
* outputs any errors to the shell

```
2>file
```
* outputs any errors to the file within the shell folder
 * if there are no errors from the command there will be no output in the file


* A Makefile is included with our shell project submission. Simply run 'make' and then './shell' to run our shell.

* Tested on Ubuntu 14.04.2 64-bit and the CISE lab machines. Lex and Yacc must be installed.


#### Not Implemented
Name completion
* File name completion has not been implemented in our shell.

Input file parsing
* The shell does not parse input from files on its own (but does allow for input redirection as per the "Features Implemented" section).

Quotations
* Spaces in quotations are not permitted when setting an environment variable or alias.

Commands
* We dont allow the user to run the following commands:
```
bc : yes
```



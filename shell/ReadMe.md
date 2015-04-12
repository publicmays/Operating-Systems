Operating-System TA's please read the ReadMe.md in better format here at this link :
https://github.com/katlew3/Operating-Systems/tree/master/shell#configuration

#### Configuration
_Install flex and bison_
```
sudo apt-get install flex
sudo apt-get install bison
```
> flex installs lex
> bison installs yacc

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

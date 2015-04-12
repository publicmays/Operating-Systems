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

#### Implemented

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
* I/O and error redirection, as well as background processing through:
	```
	> >> < 2> 2>&1 &
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

#### Not Implemented
* File name completion has not been implemented in our shell.
* Colon-seperated words for setenv























# Shell
Shell implementation in C with python embedding

Requirements
--------
```
gcc compiler
Python 2.7
```
Features
--------

* Basic commands: `exit`,`ls`, `pwd`, `clear` and `cd`
* Program invocation with forking and child processes
* Aliasing Feature
* I/O redirection (use of `dup2` system call) limited to the following:  
        `<cmd> <args> > <output>`  
        `<cmd> <args> < <input> > <output>`
* Background execution of programs with `&`
* Piping implemented (`<cmd1> | <cmd2>`) via `pipe` and `dup2` syscalls. Multiple piping is allowed.
* Editor implemented. Type (`editor`) in the shell to enable text editor feature.
	

Test
--------
```
gcc shell.c

```

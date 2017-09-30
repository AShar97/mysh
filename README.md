# mysh
This is a readme file for MYSH : A simple shell created in c by Ayush, Yugansh & Ojasvin. Period.

1) General Description
2) Compiling
3) Flow of control
4) Other details

---

### 1) General Description

Three main steps for a shell is:- 
1. Get the command from standard input.
2. Parse it into arguments and function.
3. Execute required job.

### 2) Compiling

Execute shell code by typing :-
```shell
$ make mysh
```

Clean the output by typing :-
```shell
$ make clean
```

### 3) Flow of Control

Execute shell code by typing :-
```shell
$ ./mysh
```

Program begins with executing "mysh_userinterface();" in main(), which is a continuos loop, breaks on status FALSE. In this "mysh_read_line(); " reads a line from the keyboard, mysh_split_line() splits it into command + arguments amd "mysh_execute();" execute the command and looping continues until exit is encountered.

In mysh_execute() it runs two types of command one is builin and other is default terminal command. For eg, cd is a builtin command which cannot be executed by a child process forking system because it will only change the directory of the child process which executes it. Parent process directory is not changed.

### 4) Other details

timerun command -- >  this command is used to kill a running process after a particular time.

Syntax :- timerun <time> <process_name> 

eg :-  timerun 2 ./a.out  

this command will execute ./a.out for two seconds and then kill it.

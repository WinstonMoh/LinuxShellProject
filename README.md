# LinuxShellProject

## Introduction
The project entails creating a simple Linux shell program..a program which acts as a command line user interface. The Linux command line on the Pi is one such shell program.

The shell contains some built-in commands (mostly to handle bookkeeping and for showing information) and has the ability to execute other programs. Many/most Linux commands are programs or parts of programs that live somewhere in the Linux filesystem. For example, the ls command lives in the /bin directory. A search path tells where the shell is supposed to search for programs to execute. You can see your search path on the Pi by typing echo $PATH...it will print a string of colon-delimited filesystem paths.

When the shell starts up it initializes its internal data structures and then repeatedly displays a simple user prompt. It then waits for the user to type a command line at the prompt. The shell we're going to write (and Bash, sh, and most other Linux shells) expects the user's command line to be of the form: command arg arg ... where the command to be executed is the first word on the line and the remaining words are arguments which are supplied to that command.

Commands may be internal or external. Internal commands must be interpreted and performed by the shell itself. External commands should be executed via the fork()/execv() approach that we discussed in class. Note that the execvp() function may also be used. Don't use any of the other functions in the exec() family in this assignment.

The majority of commands that ever get entered at a command line are external commands. This means that the shell has to find the file, prepare the list of parameters for the command, and then cause the command to be executed using the parameters. Note that "command" includes programs written by the user. This is what has happened when we typed ./prog to execute the programs we built using the assembler.

## FUNCTIONALITIES
### prompt, history, and absolute paths
Setting up environmental variables.

### Relative paths
cd and ls commands.

### Search paths
whereis commands

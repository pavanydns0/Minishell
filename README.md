Structure:
MiniShell/
│── README.md
│── LICENSE
│── Makefile
│── src/
│── include/
│── obj/
│── docs/
│     ├── architecture.png
│     ├── flowchart.png
│── images/
│     ├── demo.gif
│     ├── screenshot1.png
│── .gitignore

MiniShell - Linux Command Line Interpreter
Overview

MiniShell is a simplified implementation of the Linux Shell developed in C using Linux System Calls.

The project demonstrates how a real shell like bash accepts user commands, creates child processes, executes programs, manages jobs, handles signals, and controls foreground/background execution.

This project helped me understand the internal working of Linux process management and Operating System concepts rather than just using terminal commands.

Problem Statement

Whenever we type commands like

pwd
ls
cat file.txt
sleep 10 &

Linux executes them immediately.

As an Embedded/Linux Engineer, I wanted to understand:

How does Linux execute commands?
How are child processes created?
What happens when Ctrl+C is pressed?
How are background jobs managed?
How does the shell know whether to wait or accept another command?

This curiosity motivated me to build my own MiniShell.

Features

✔ Interactive Shell

✔ Command Parsing

✔ Execute External Commands

✔ Built-in Commands

cd
pwd
exit
jobs
fg
bg

✔ Process Creation using fork()

✔ Program Execution using execvp()

✔ Foreground Process Handling

✔ Background Process Handling

✔ Job Control

✔ Signal Handling

Ctrl+C
Ctrl+Z

✔ Zombie Process Handling

✔ Linux System Calls

Technologies Used:

C Programming	
Linux	Operating System
GCC	Compiler
Makefile:	Build Automation
fork():	Process Creation
execvp():	Execute Commands
waitpid():	Process Synchronization
signal():	Signal Handling
kill():	Process Control

Project Workflow
                User Types Command

                       │
                       ▼

               Read User Input

                       │
                       ▼

               Parse the Command

                       │
          ┌────────────┴─────────────┐
          │                          │
          ▼                          ▼

 Built-in Command?             External Command?

          │                          │

    Execute Directly            fork()

                                      │
                                      ▼

                            Child Process Created

                                      │

                                   execvp()

                                      │

                           Execute Linux Program

                                      │

                              waitpid() (FG)

                           Return Prompt Again
Process Flow
User

↓

MiniShell

↓

Tokenizer

↓

Command Parser

↓

Built-in ?

↓

YES ---------------- Execute Built-in

NO

↓

fork()

↓

Child

↓

execvp()

↓

Linux Program

↓

Output

↓

Parent waits

↓

Prompt

Linux Concepts Covered:

Process Creation
Parent and Child Process
Process Scheduling
Process IDs
Job Control
Signals
System Calls
Foreground Process
Background Process
Zombie Process
Process Synchronization
How to Compile

Compile all source files using GCC

gcc *.c

or

make
How to Run

Run the executable

./a.out

or

./minishell
Sample Execution
$ gcc *.c

$ ./a.out

MiniShell$

MiniShell$ pwd
/home/pavan/MiniShell

MiniShell$ ls
main.c parser.c signal.c Makefile

MiniShell$ date
Sun Jul 19 12:45:01 IST 2026

MiniShell$ sleep 20 &

[1] Running

MiniShell$ jobs

[1] sleep 20 Running

MiniShell$ fg 1

sleep 20

MiniShell$

MiniShell$ Ctrl+Z

Stopped

MiniShell$ bg 1

Running

MiniShell$ exit
Supported Commands
pwd

ls

echo

cat

date

sleep

jobs

fg

bg

cd

exit

System Calls Used:

fork()

execvp()

waitpid()

kill()

signal()

getpid()


setpgid()

pipe()

read()

write()

Built-in Commands:

Command	Description
cd	Change Directory
pwd	Print Working Directory
jobs	Display Background Jobs
fg	Bring Job to Foreground
bg	Resume Background Job
exit	Exit MiniShell

Example
Current Directory
MiniShell$ pwd

/home/user/project
List Files
MiniShell$ ls

main.c
parser.c
signal.c
Makefile
Background Job
MiniShell$ sleep 30 &
View Jobs
MiniShell$ jobs
Bring Job to Foreground
MiniShell$ fg 1
Resume Job
MiniShell$ bg 1

Project Architecture
             +----------------+
             |    User Input  |
             +----------------+
                     |
                     ▼
             Command Parser
                     |
        +------------+-------------+
        |                          |
        ▼                          ▼
  Built-in Commands        External Commands
        |                          |
        ▼                          ▼
 Execute Directly             fork()
                                   |
                                   ▼
                               execvp()
                                   |
                                   ▼
                           Linux Executable
                                   |
                                   ▼
                              Process Control
                                   |
                                   ▼
                             Signal Handling
                                   |
                                   ▼
                            Return to Prompt
Learning Outcomes

Through this project, I gained hands-on experience with:

Linux Process Management
System Programming
Process Scheduling
Job Control
Signal Handling
Parent-Child Communication
Shell Internals

Author

YDNS Pavan

Embedded Systems Engineer

C | Linux | Linux Internals | Embedded C | Microcontrollers | System Programming

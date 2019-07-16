# mykernel
Kernel and Shell Simulator

### Table of Contents

1. [Project Description](#Project)
2. [File Description](#files)
3. [Licensing, Authors, and Acknowledgements](#licensing)

## Project Description<a name="Project"></a>

This project is a Kernel and Shell simulator written in c.

Some of the concepts implemented in this project are:
- Task Scheduling
- Quanta and I/O interrupts
- Shell commands
- Thread simulations
- CPU scheduling
- I/O scheduling
- File System simulator
- RAM and PCB simulations

Shell Commands:
```
  help: Display all the commands\n");
  quit: Exits / terminates the shell with “Bye!”
  set VAR STRING: Assigns a value to shell memory
  print VAR: Prints the STRING assigned to VAR
  run SCRIPT.TXT: Executes the file SCRIPT.TXT
  exec p1.txt p2.txt p3.txt: Executes concurrent programs
  mount partitionName number_of_blocks block_size: mounts a partition
  write filename [a bunch of words]: writes to a file
  read filename variable: reads a file block into variable
```
## File Descriptions <a name="files"></a>
- /src/

## Licensing, Authors, Acknowledgements<a name="licensing"></a>
This was done as part of Operating Systems at McGill University.

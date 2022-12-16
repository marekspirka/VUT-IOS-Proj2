# Summary 6/15 
##### (idk why bcs it works)
# Description of Tasks (Building H2O)

### The assignment is inspired by the book Allen B. Downey: The Little Book of Semaphores
Water molecules are made of two hydrogen atoms and one oxygen atom. There are three types in the system
processes: (0) main process, (1) oxygen and (2) hydrogen. After the processes are created, the representing processes
it ranks oxygens and hydrogens in two queues—one for oxygens and one for hydrogens. Always from the beginning of the queue
one oxygen and two hydrogens come out to form a molecule. Only one can be created at a time
molecule. After its formation, space is freed for other atoms to form another molecule. processes,
which formed the molecule subsequently ends. When there are no longer enough oxygen atoms available
or hydrogen for the next molecule (and no more will be created by the main process) are
all remaining oxygen and hydrogen atoms are released from the fronts and the processes are terminated.
## Detailed job specification
### Startup:
$ ./proj2 NO NH TI TB <br>
• NO: Number of oxygens <br>
• NH: Number of hydrogens <br>
• TI: The maximum time in milliseconds that an oxygen/hydrogen atom waits after its creation before it 
queue up to create molecules. 0<=TI<=1000 <br>
• TB: Maximum time in milliseconds required to form one molecule. 0<=TB<=1000

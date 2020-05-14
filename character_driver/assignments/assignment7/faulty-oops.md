
## DUMP VALUE FOR echo “hello_world” > /dev/faulty command: 

Unable to handle kernel NULL pointer dereference at virtual address 0000000000000000 <br />
Mem abort info:									     <br />
  ESR = 0x96000046 <br />
  Exception class = DABT (current EL), IL = 32 bits <br />
  SET = 0, FnV = 0 <br />
  EA = 0, S1PTW = 0 <br />
Data abort info: <br />
  ISV = 0, ISS = 0x00000046 <br />
  CM = 0, WnR = 1 <br />
user pgtable: 4k pages, 39-bit VAs, pgdp = (____ptrval____) <br />
[0000000000000000] pgd=000000004599b003, pud=000000004599b003, pmd=0000000000000000 <br />
Internal error: Oops: 96000046 [#1] SMP <br />
Modules linked in: faulty(O) hello(O) scull(O)  <br /> 
CPU: 0 PID: 144 Comm: sh Tainted: G           O      4.19.16 #1       <br />
Hardware name: linux,dummy-virt (DT)      <br />
pstate: 80000005 (Nzcv daif -PAN -UAO) <br />
pc : faulty_write+0x8/0x10 [faulty] <br />
lr : __vfs_write+0x30/0x170 <br />
sp : ffffff800889bd40 <br />
x29: ffffff800889bd40 x28: ffffffc005fab980 <br /> 
x27: 0000000000000000 x26: 0000000000000000 <br />
x25: 0000000056000000 x24: 0000000000000015 <br />
x23: 00000000004c8740 x22: 0000000000000000 <br />
x21: ffffff800889be60 x20: ffffffc005fa7400 <br />
x19: ffffff80086e8000 x18: 0000000000000000 <br />
x17: 0000000000000000 x16: 0000000000000000 <br />
x15: 0000000000000000 x14: 0000000000000000 <br />
x13: 0000000000000000 x12: 0000000000000000 <br />
x11: 0000000000000000 x10: 0000000000000000 <br />
x9 : 0000000000000000 x8 : 0000000000000000 <br />
x7 : 0000000000000000 x6 : ffffff8000565000 <br />
x5 : ffffff8000563000 x4 : ffffff80086e8548 <br />
x3 : ffffff800889be60 x2 : 0000000000000012 <br />
x1 : 0000000000000000 x0 : 0000000000000000 <br />
Process sh (pid: 144, stack limit = 0x(____ptrval____)) <br /> 
Call trace: <br />
 faulty_write+0x8/0x10 [faulty] <br />
 vfs_write+0xa8/0x1a0 <br />
 ksys_write+0x60/0xe0 <br />
 __arm64_sys_write+0x18/0x20 <br /> 
 el0_svc_common+0x60/0x100 <br />
 el0_svc_handler+0x68/0x80 <br />
 el0_svc+0x8/0xc <br />
Code: bad PC value <br />
---[ end trace f309fbef3516e60d ]--- 


## Analysis : 
	
In faulty.c file the *(int *)0 = 0; instuction dereferences a NULL pointer which is never a valid pointer value. This results into a fault which is pasted as above.The calling process is killed and new login is started.When an invalid pointer is dereferenced, the paging mechanism fails to map the pointer to a physical address and the processor signals page fault to the operating system. 4

The faulty(O) hello(O) scull(O) modules were linked during the occurance of the fault.The program counters pointes to faulty module which means that it is responsible for occurence of fault.The PC isllustrates that it was pointing to 8 bytes into the function faulty_write that is 10 bytes long.

The exception synrome register which stores the reason for the expection has value 0x96000046 with data abort exception class.

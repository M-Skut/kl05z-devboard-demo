.text
.align 4

.global _start
.global _exit

_start:
        bl main /* Fall-through to exit */

_exit:
/* Will cause DebugMon or HardFault if debugger is not available and the main program exits */
        bkpt #0

.type _start,function
.size _start,_exit-_start

.type _exit,function
.size _exit,.-_exit

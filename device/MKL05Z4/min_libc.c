/**
 * Files to serve as a simples replacement of original libc to save as many bytes as possible 
 */

extern int main();
int (*g)(void) = main;

__attribute__((weak)) void _start(void)
{
    __asm volatile(
    "mov lr, %1\n\t"
    "bx %0\n\t"
    : : "r" (main), "r" (g));

}

__attribute__((weak, noreturn)) void _exit(int a __attribute__((unused))){
	__builtin_unreachable();
}
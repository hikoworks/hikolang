# Large Data-stack

## Size

On 64 bit systems the memory mapping is 2 GB in size, which is
substantially larger than the normal stack.

## Guard

At the end of the data stack, reserve a 4096-byte guard region with access 
protection enabled. Any access to this region must cause a segmentation fault.

Every function must check for stack overflow. The compiler generates the check
according to the allocation type:
 * **Fixed-size frames smaller than 4096 bytes:** Probe the newly allocated
   frame by writing to `base-pointer - 1`. This detects overflow when the
   probe reaches the guard page, provided the compiler maintains the
   required allocation and probing invariants.
 * **Fixed-size frames of 4096 bytes or larger, and dynamic stack allocation:**
   Calculate the proposed new base pointer and compare it against a thread-local
   variable containing the data-stack limit. If the allocation exceeds the
   limit, deliberately write to the guard page to trigger a segmentation fault.

After the deliberate faulting write, emit `ud2` on x86-64 as a fallback in case
the write unexpectedly returns.

The compiler must ensure that large allocations cannot skip over the guard
 region without detection. The base pointer must not be committed to an
 out-of-bounds value before the overflow check succeeds.

The following optimizations can improve performance:
 * If a function does not call another function and its fixed-size frame is
   smaller than 4096 bytes, it may omit the explicit `base-pointer - 1` probe,
   provided all memory accesses remain within the valid allocated frame.
 * The compiler may reorder local variables so that normal writes before a
   function call naturally touch `base-pointer - 1`, eliminating the need for
   a separate probe.

On architectures where the fault handler can identify the faulting address,
the runtime can distinguish a guard-page fault from an unrelated segmentation
fault. It can then report a "stack overflow" error to the console. A modal
dialogue may be shown later from a safe execution context, rather than directly
from the signal handler.

## x86-64

```asm
foo:
        push rbp
        lea rbp, [rbp + <frame_size>]

        ; 1st local variable
        mov rax, [rbp - 8]

        pop rbp
        ret
```

When the function uses dynamic allocation the fixed frame should be loaded in a
separate register.

```asm
foo:
        push rbp
        lea rbp, [rbp + <frame_size>]
        mov r10, rbp

        ; alloca like allocation
        mov rax, <alloca_size>
        lea rbp, [rbp + rax]

        ; 1st local variable
        mov rax, [r10 - 8]

        pop rbp
        ret
```

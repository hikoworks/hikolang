# Concurency and Memory Model

The language adopts the C++ memory model for data races, atomic operations,
memory ordering, synchronization, and thread lifetimes, extended to include
hardware as an external execution agent accessing shared state.

A data race occurs when conflicting accesses to the same memory location execute
in different threads or hardware agents, at least one access is non-atomic, and
neither access happens-before the other. A data race results in undefined
behavior.

Concurrent access to shared mutable state must use atomic operations,
hardware-I/O operations where applicable, or appropriate synchronization.

The language provides the C++ memory-ordering semantics, including `relaxed`,
`acquire`, `release`, `acq_rel`, and `seq_cst` ordering for atomic and
hardware-I/O operations, subject to the constraints of each operation.
Hardware-I/O operations may establish synchronization with a device according to
their specified semantics.

Atomic operations are indivisible and participate in the modification order of
their atomic object. Hardware-I/O operations perform the prescribed device
interaction, which may require specific instructions or sequences of accesses to
multiple I/O ports or MMIO (Memory Mapped I/O). Their contracts define access
width, ordering, synchronization, and completion guarantees. Such operations
must not be eliminated, duplicated, combined, or replaced by cached values
unless explicitly permitted by their contracts.

Hardware may access ordinary memory through DMA. Such accesses participate in
the extended memory model, and synchronization must ensure the required
visibility and ordering between software and the device. Non-coherent DMA may
additionally require explicit cache maintenance.

In the absence of applicable ordering constraints, the compiler and execution
environment may optimize ordinary memory accesses as permitted by the memory
model. In particular, multiple reads from the same memory location may reuse a
previously loaded value, and multiple writes may be combined or eliminated.
Memory accesses may also be reordered.

Fences constrain the ordering of ordinary-memory, atomic, and hardware-I/O
operations according to their specified semantics. A fence does not
independently establish a happens-before relationship between agents;
synchronization must be established through the atomic operation, hardware-I/O
operation, or synchronization primitive defined by the memory model.

Ordering alone does not guarantee device-operation completion, device
acknowledgement, or DMA visibility. These guarantees are provided only by
operations or platform contracts that explicitly define them.


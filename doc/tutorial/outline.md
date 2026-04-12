0. Program Structure (Ground the Reader Early)
 - File structure and comments (//)
 - The prologue (import, module, program)
 - Smallest working program
 - Compile vs run
 
1. First Values and Execution
 - Integer and float literals
 - Basic arithmetic
 - Printing / observable output

👉 No types mentioned explicitly yet

2. Variables (First Contact with the System)
 - var declarations
 - Copying values (var b = a)
 - let (sealed variables)

3. Ranged Integers (Introduce the First “Weird Rule”)
 - What int[4..=10] means
 - Literals have exact ranges (10 → int[10..=10])
 - Assignment failures due to range mismatch
 - Why this exists (safety + optimization)

4. Conversions (Make It Usable)
 - Widening conversion (:)
 - Narrowing conversion (:!)
 - Exact matching (:=)
 - Super-type matching (:^)

👉 This is a core chapter cluster

5. Type Annotations & Inference
 - Adding explicit types (var x = 10 : int[0..<100])
 - Removing annotations (inference)
 - How inference decides ranges
 - When inference fails or surprises

6. References and Mutability
 - Mutable references (&mut)
 - Const views (&const)
 - ref and view sugar
 - Mutation through references

7. Functions (Keep It Simple First)
 - Basic function definition (fn)
 - Parameters and return types
 - Calling functions
 - Universal call syntax (a.foo())

8. Functions Are Values
 - Assigning functions to variables
 - Desugared function definitions
 - Functions as arguments

👉 Still avoid “templates” wording

9. Control Flow
 - Conditionals (if)
 - Boolean expressions
 - Returning values

10. Structs (Data Shapes)
 - Defining a struct
 - Fields and defaults
 - Creating instances
 - Accessing fields

11. Methods (Multiple Forms)
 - Member functions inside structs
 - External method definitions
 - Assigning methods dynamically
 - Unified mental model

12. Reveal: Everything Is a Template
 - Functions specialize per call
 - No single “fixed” function exists
 - Code generation model

13. Type Templates
 - Generic structs (struct foo[T])
 - Type parameters
 - Specialization behavior

14. Reveal: Types Are Values
 - Assigning types to variables
 - Passing types as arguments
 - Constructing types dynamically

15. Types Are Functions (Desugaring)
 - struct == function returning a type
 - Type construction flow
 - Why this design exists

👉 This must come after templates, or it won’t land

16. Type Mutability
 - Modifying a type after creation
 - Adding fields or behavior
 - When this is useful vs dangerous

17. Lazy Freezing (Critical Concept)
 - When a type becomes frozen
 - “First use” rule
 - Compile-time guarantees
 - Errors from late mutation

👉 This is one of your most important chapters

18. Functions Are Mutable Too
 - Functions as mutable values
 - Modifying behavior
 - Freezing rules for functions

19. Units and Domains
 - Unit-tagged values (5.0 m)
 - Unit expressions (m/s)
 - Unit matching in function parameters
 - Domain systems (SI, screen space, etc.)
 - Cross-domain conversion (e.g., PPI)

20. Collections & Iteration
 - Arrays / collections
 - Iteration patterns
 - Interaction with ranges

21. Implicit Arguments
 - $arguments concept
 - Forwarding implicit parameters

 - When explicit passing is required
22. Error Handling Model
 - result[T] enum
 - Returning errors
 - Handling errors locally
 - Why exceptions are values

23. Modules, Packages, and Build System
 - Modules and namespaces
 - Versioned modules
 - Packages as git repositories
 - Built-in package manager
 - Conditional compilation

24. Putting It All Together
 - How compilation works (high level)
 - Interaction of:
 - templates
 - type mutation
 - freezing
 - inference
 - Performance implications


🧠 Critical Observations (You Should Not Ignore)
1. Your language has two “truth layers”
Surface: looks like a normal systems language
Reality: everything is late-bound until frozen

If you reveal that too early → confusion
If you reveal it too late → betrayal

This outline times it so the user expects it

2. The real backbone is:

range → conversion → inference → templates → mutation → freezing

If you break that chain, users won’t understand errors.

3. The hardest concept isn’t templates—it’s freezing

Users will constantly ask:

“Why can’t I modify this anymore?”
“Why did this compile yesterday?”

So your tutorial must repeatedly reinforce:

“Using something makes it real (frozen).”

4. Units are deceptively complex

They interact with:

types
inference
functions

Good call placing them late, not early.
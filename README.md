# CBigInt (C++23)

Arbitrary-precision signed integer implementation in modern C++ (`C++23`), storing values in little-endian base-$2^{32}$ (`uint32_t`) limbs with 64-bit intermediate carry/borrow arithmetic.

## Features
- **Internal Representation:** Little-endian `std::vector<uint32_t>` in base $2^{32}$ with sign tracking.
- **Constructors & Assignment:** Implicit zero initialization, `int` (including `INT_MIN` edge-case handling), and validated decimal `std::string` parsing.
- **Arithmetic Operators:** Addition (`+`, `+=`) and multiplication (`*`, `*=`) across any combination of `CBigInt`, `int`, and `std::string`.
- **Comparisons:** Full relational operator suite (`==`, `!=`, `<`, `<=`, `>`, `>=`) supporting mixed operand types.
- **Stream I/O:** Formatted output (`<<`) in both decimal (`std::dec`) and hexadecimal (`std::hex`), and validated stream extraction (`>>`).

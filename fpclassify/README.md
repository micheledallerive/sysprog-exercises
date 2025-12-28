# IEEE 754 Floating-Point Classifier

## Goal

Implement a custom version of the standard `fpclassify` function. This exercise focuses on **low-level bit manipulation** and the internal representation of the **IEEE 754 standard** for single-precision floating-point numbers.

You must categorize a 32-bit `float` into its five primary classes (zero, subnormal, normal, infinite, and NaN) based solely on its raw binary structure.

---

## Constraints and Requirements

* **No Standard Math Macros**: You are strictly forbidden from using `<math.h>` classification macros like `isnan()`, `isinf()`, or `isfinite()`. Either way, importing `<math.h>` will result in a compilation error.
* **Bitwise Operations**: Your implementation must utilize bitwise operations to dissect the float's binary representation.
* **Manual Masking**: You are expected to identify and isolate the relevant bit fields (sign, exponent, and mantissa) manually using shifts and masks.

---

## Classification Rules

Your implementation must return the following constants based on the bit pattern:

* **`FP_ZERO`**: Represents both positive and negative zero.
* **`FP_SUBNORMAL`**: Denormalized numbers.
* **`FP_NORMAL`**: The standard range of floating-point numbers.
* **`FP_INFINITE`**: Values representing positive or negative infinity.
* **`FP_NAN`**: "Not-a-Number" results.

---

## Testing Your Code

The provided test suite includes 20 test cases covering:
* **Zero and Sign**: Distinguishing between `0.0f` and `-0.0f`.
* **Infinities**: Handling overflow and explicit infinity constants.
* **NaNs**: Detecting various "Not-a-Number" bit patterns.
* **Normal/Subnormal Boundaries**: Verifying the transition points at the edges of the representable range (e.g., `FLT_MIN`).

To run the tests:

```bash
make
./test
```

or

```bash
make run
```

You should see:

```text
* Suite fpclassify_suite:
....................

20 tests - 20 pass, 0 fail, 0 skipped
```

---

## Files You'll Modify

* **`lib.c`**: Implement the `fpclassify` logic.

## Files Provided

* **`lib.h`**: Header containing the enum and function prototype.
* **`greatest.h`**: The unit testing framework.
* **`Makefile`**: Build instructions.

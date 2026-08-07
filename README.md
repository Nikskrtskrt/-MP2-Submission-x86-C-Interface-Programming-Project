# MP2 Submission x86-64 / C Interface Programming Project
SERRA, MARY NICOLE S.

Grayscale image conversion (double-precision float pixels → uint8 pixels)
implemented in C (I/O, memory, timing) and x86-64 assembly (the actual
conversion, using scalar SIMD floating-point instructions).

## Files

- `main.c` reads height/width and pixel data, allocates buffers, times
  the assembly routine over 30 runs, and prints the output.
- `imgcvt.asm` implements `imgCvtGrayDoubleToInt`, the conversion routine,
  using scalar SSE2 instructions (`movsd`, `mulsd`, `cvtsd2si`).
- `main.exe` prebuilt Windows executable (optional convenience; can be
  rebuilt from source, see Build below).

## Build

```
nasm -f win64 imgcvt.asm -o imgcvt.obj
gcc main.c imgcvt.obj -o main.exe
```
(or the MSVC/mingw equivalent any Windows x86-64 toolchain works.)

## Run

Manual input:
```
main.exe
3 4
0.25 0.35 0.45 0.33
0.55 0.65 0.75 0.33
0.85 0.95 0.15 0.33
```

Random input, for timing runs:
```
main.exe r 10 10
main.exe r 100 100
main.exe r 1000 1000
```

## Correctness Check

The assembly routine uses `cvtsd2si`, which rounds to the nearest integer
(round-half-to-even) per IEEE 754 / the CPU's default rounding mode. This
was verified two ways:

**1. Against the assignment's sample image (3x4, values 0.25–0.95):**

| Pixel | f    | f*255 (exact) | Program output | Spec's sample output |
|-------|------|----------------|------------------|------------------------|
| most  | -    | -              | matches          | matches                |
| (0,2) | 0.45 | 114.75         | **115**          | 114                    |
| (2,0) | 0.85 | 216.75         | **217**          | 216                    |

All 10 other pixels match exactly. The two that don't (`0.45` and `0.85`)
both land on `x.75`, which is strictly closer to the next integer up, so
round-to-nearest correctly produces 115 and 217. The spec's table appears
to use truncation for just these two entries, while the rest of the table
(e.g. `0.25 -> 63.75 -> 64`) matches rounding, not truncation, no single
rounding rule reproduces every value in the given sample. This looks like
a typo in the assignment's example rather than an error in the program;
round-to-nearest was kept since it's mathematically correct and consistent
across every other value in the table.

**2. Against an independent Python reference implementation** using the
same round-half-to-even rule, run on a 10x7 randomized image (70 pixels,
seeded for reproducibility). Every pixel matched exactly, confirming the
assembly routine's rounding and clamping logic is correct in general, not
just on the hand-checked sample.

**Screenshot (actual program run):**

<img width="545" height="376" alt="image" src="https://github.com/user-attachments/assets/a0349020-0a3e-4a05-9667-aba4140b8174" />

## Performance

Measured on the submitter's machine, 30 runs averaged per size:

| Image size  | Avg time over 30 runs |
|-------------|------------------------|
| 3 x 4       | 0.000000073 s |
| 10 x 10     | 0.000000303 s |
| 100 x 100   | 0.000020317 s |
| 1000 x 1000 | 0.000949607 s |

Runtime scales roughly linearly with pixel count, consistent with the O(n)
scalar loop: each pixel is one `movsd` / `mulsd` / `cvtsd2si` plus a
compare-based clamp, with no dependency between pixels. The jump from
10x10 to 100x100 to 1000x1000 tracks the ~100x growth in pixel count each
step, showing the per-pixel cost stays flat as the image grows rather than
degrading, no evidence of cache-related slowdown even at 1,000,000 pixels.

## Video

Walkthrough of source code, compilation, and execution: [https://drive.google.com/file/d/1z2yfh9knCBAiOagsW7qbTQ5Kn4Ab5dgz/view?usp=sharing]

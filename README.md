## Correctness Check
 <img width="542" height="427" alt="Screenshot 2026-08-07 190514" src="https://github.com/user-attachments/assets/d1c5231e-83f0-4fd2-ad18-61d7c5b55fd1" />
 
The assembly routine uses `cvtsd2si`, which rounds to the nearest integer
(round-half-to-even) per IEEE 754 / the CPU's default rounding mode. This was
verified two ways:

1. **Against the assignment's sample image** (3x4, values 0.25–0.95):

   | Pixel | f      | f*255 (exact) | Program output | Spec's sample output |
   |-------|--------|---------------|-----------------|------------------------|
   | most  | -      | -             | matches         | matches                |
   | (0,2) | 0.45   | 114.75        | **115**         | 114                    |
   | (2,0) | 0.85   | 216.75        | **217**         | 216                    |

   All 10 other pixels match exactly. The two that don't (`0.45` and `0.85`)
   both land on `x.75`, which is strictly closer to the next integer up, so
   round-to-nearest correctly produces 115 and 217. The spec's table appears
   to use truncation for just these two entries while the rest of the table
   (e.g. `0.25 -> 63.75 -> 64`) matches rounding, not truncation and no single
   rounding rule reproduces every value in the given sample. This looks like
   a typo in the assignment's example rather than an error in the program;
   round-to-nearest was kept since it's the mathematically correct behavior
   and it's consistent across every other value in the table.

2. **Against an independent Python reference implementation** using the same
   round-half-to-even rule, run on a 10x7 randomized image (70 pixels, seeded
   for reproducibility). Every pixel matched exactly, confirming the assembly
   routine's rounding and clamping logic is correct in general, not just on
   the hand-checked sample.

## Performance

| Image size | Avg time over 30 runs |
|------------|------------------------|
| 10 x 10    | 0.000000183 s |
| 100 x 100  | 0.000006243 s |
| 1000 x 1000| 0.001121210 s |

Runtime scales roughly linearly with pixel count (~1.1 microseconds per
1,000 pixels), consistent with the O(n) scalar loop each pixel is one
`movsd`/`mulsd`/`cvtsd2si` plus a compare-based clamp, with no
inter-pixel dependency, so the cost per element stays flat as the image
grows and the small jump between 10x10 and 100x100 is mostly fixed
loop/call overhead rather than the per-pixel work itself.

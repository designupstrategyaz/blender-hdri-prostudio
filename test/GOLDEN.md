# Test harness — golden baseline

Recorded from the post-Phase-1 engine (`main` after PR #1 merged), before any
Phase 2 render optimization.

**Scenario:** 256×128 RGBA float image, 3 flat-content lights — see
`harness.cpp` for exact parameters.

```
pixel sum : 41860.575087
byte hash : 5fa539c9163fbf2c

light 1 centre   (128, 64) = 1.00000 0.30000 0.20000 1.00000
light 2 centre   ( 80, 50) = 0.25000 1.05000 0.35000 1.00000
light 3 centre   (180, 80) = 0.30000 0.40000 1.00000 1.00000
background       (230,  20) = 0.05000 0.05000 0.05000 1.00000
between lights   (104, 57) = 0.05000 0.05000 0.05000 1.00000
```

Any Phase 2 optimization (bounding-box restriction, contribution caching)
must reproduce these values **exactly** — the harness output is deterministic
regardless of thread count, because render and composite work is partitioned
into disjoint pixel ranges.

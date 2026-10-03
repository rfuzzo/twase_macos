# Address table

Build: Feral 1.6.1 RC2, `CFBundleVersion 480285.103778`, `LC_UUID 392D6F66-9183-329E-8A98-8C90FC828326`.
Offsets are relative to the image base `0x100000000` (add the ASLR slide at runtime).

| Name | PC RVA (empire.retail.dll) | Mac offset | Anchor | Confidence | Notes |
|---|---|---|---|---|---|
| BitSetCrashAddr (unit size patch) | `0x0091CB57` (`cmp edi,40h; jnb`) | `0x1A51C58` (`b.eq`) | xref to `"bitset test argument out of range"` (5 sites). Only this one has the `mov x8,#-1` mask → vcall(vtable+0x30, &mask) → list walk → `cmp x20,#0x40` shape | high | expected `40 26 00 54` (`b.eq 0x101a52120` → throw). Patch `80 00 00 54` (`b.eq 0x101a51c68`, include branch). The preceding `cmp x20,#0x40` is at `0x1A51C54` |

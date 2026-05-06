# Jerrieee

Small firmware project for the Micromouse platform (STM32G0 series).

**Prerequisites:**
- Toolchain: `arm-none-eabi-gcc` (and related binutils/openocd).

**Get headers (submodule)**: the repository expects ST's device headers and drivers in `chip_headers`.
Run from the repo root:

```bash
git submodule update --init --recursive
```

This will populate `chip_headers` with the STM32Cube content used by the Makefile.

**Build**:

```bash
make
```

The `Makefile` uses the linker script bundled in the headers at:
`chip_headers/Projects/NUCLEO-G070RB/Templates_LL/STM32CubeIDE/STM32G070RBTX_FLASH.ld`.

If you prefer copying the linker script into the repo root, copy that file to `STM32G070RBTX_FLASH.ld` and adjust `LDFLAGS` accordingly.

**Flash**:

```bash
make flash
```

This runs `openocd` with the ST-Link interface and the STM32G0 target config.

**Notes**:
- The project targets the `STM32G070` (Cortex-M0+). The `Makefile` contains the compiler flags used for compilation (nano.specs, soft float, stack-usage helpers).
- The build may produce `.su` files (stack usage reports). They are diagnostic and cleaned by `make clean`.
- To pin `chip_headers` to a specific ST release, enter `chip_headers` and checkout the desired tag/commit, then commit the submodule change in this repo.

# Infinidoge's `os-tutorial` implementation

## Based on [cfenollosa/os-tutorial](https://github.com/cfenollosa/os-tutorial), targeting `i386`

For my Operating Systems class, we were presented with kernel code to build and tinker around with.
In my typical fashion, I decided to go overboard and implement the entire build process in Nix, rather than mess around with a fragile VM environment.
This flake builds a cross compiler/linker toolchain, as well as a cross-targeted gdb package, most notably with GCC 5.4, binutils 2.26.1.

## Usage

Each of the folders in `./kernel` are built as a flake output.
A given folder can be compiled with `nix build .#name`, such as `nix build .#01_gdb`.

Additionally, there are the `gcc`, `binutils`, and `gdb` outputs for the cross-compiler versions of those programs.
For easy access, these (plus `qemu` and `nasm`) are included in the devshell, for which `direnv` is also provided.

Finally, for easy running, each of the kernel folders also have a series of run scripts built for use with `nix run`:

- `name-run` runs the kernel in normal QEMU
- `name-curses` runs the kernel in QEMU using curses
- `name-debug` runs the kernel in QEMU using curses, with the `gdb` target enabled
- `name-gdb` runs `gdb` with the kernel elf, kernel sources, etc, included. Automatically targets the `gdb` host from `name-debug`
- `name` is an alias to `name-curses`

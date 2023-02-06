{
  description = "Subset of the os-tutorial, built with Nix";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    nixpkgs-old.url = "github:NixOS/nixpkgs/63c7b4f9a7844f0bc84d008b810375eb0fba6b2f";
    nixpkgs-old.flake = false;
  };

  outputs = inputs@{ self, nixpkgs, nixpkgs-old, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        lib = pkgs.lib;

        nixpkgs-old-patched = pkgs.applyPatches {
          name = "nixpkgs-old-patched";
          src = nixpkgs-old;
          patches = [
            ./fix-binutils-priority.patch
            ./use-binutilsCross-in-gcc.patch
          ];
        };

        pkgs-old = (import nixpkgs-old-patched {
          inherit system;
        });

        cross = {
          config = "i386-elf";
          libc = "glibc";
        };

        binutils = pkgs-old.binutils.override { inherit cross; };

        gcc = pkgs-old.gcc5.cc.override {
          inherit cross;
          binutilsCross = binutils;
          langCC = false;
        };

        gdb = pkgs-old.gdb.override { target = cross; };

        dirs = lib.filterAttrs (file: type: type == "directory") (builtins.readDir ./kernel);
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            binutils
            gdb
            nasm
            qemu
          ];
        };

        packages =
          let
            kernelPkgs = lib.mapAttrs'
              (name: _: {
                inherit name;
                value = pkgs.callPackage ./kernel/kernel.nix {
                  inherit name gcc binutils;
                  src = ./kernel + "/${name}";
                };
              })
              dirs;
          in
          kernelPkgs // { inherit gcc binutils gdb; };

        apps =
          let
            kernelAppsPre = lib.mapAttrs'
              (name: _: {
                inherit name;
                value = pkgs.callPackage ./kernel/run.nix {
                  drv = self.packages.${system}.${name};
                  inherit gdb;
                };
              })
              dirs;

            kernelApps = lib.concatMapAttrs
              (name: value: (lib.mapAttrs'
                (iname: ivalue: {
                  name = "${name}-${iname}";
                  value = {
                    type = "app";
                    program = lib.getExe ivalue;
                  };
                })
                value) // { ${name} = self.apps.${system}."${name}-curses"; }
              )
              kernelAppsPre;
          in
          kernelApps;
      }
    );
}

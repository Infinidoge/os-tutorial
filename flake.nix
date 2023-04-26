{
  description = "Subset of the os-tutorial, built with Nix";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = inputs@{ self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachSystem [ flake-utils.lib.system.x86_64-linux ] (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        lib = pkgs.lib;

        pkgs-i386 = (import nixpkgs {
          inherit system;
          crossSystem = {
            config = "i386-elf";
          };
        }).buildPackages;

        dirs = lib.attrNames (lib.filterAttrs (file: type: type == "directory") (builtins.readDir ./kernel));

        mapCallPackage = list: func: attrs:
          lib.genAttrs list (name: pkgs.callPackage func (if builtins.typeOf attrs == "lambda" then attrs name else attrs));
      in
      rec {
        devShells.default = pkgs.mkShell {
          buildInputs = (with pkgs; [
            nasm
            qemu
          ]) ++ (with pkgs-i386; [
            gcc
            binutils
            gdb
          ]);
        };

        packages =
          let
            kernelPkgs = mapCallPackage dirs ./nix/kernel.nix (name: {
              inherit name;
              inherit (pkgs-i386) gcc binutils;
              src = ./kernel + "/${name}";
            });
          in
          kernelPkgs // rec {
            inherit (pkgs-i386) gcc binutils gdb;
          };

        apps =
          let
            kernelAppsPre = mapCallPackage dirs ./nix/run.nix (name: {
              drv = self.packages.${system}.${name};
              inherit (pkgs-i386) gdb;
            });

            kernelAppsFunction =
              (name: value: (lib.mapAttrs'
                (iname: ivalue: {
                  name = "${name}-${iname}";
                  value = {
                    type = "app";
                    program = lib.getExe ivalue;
                  };
                })
                (builtins.removeAttrs value [ "override" "overrideDerivation" ])) // { ${name} = self.apps.${system}."${name}-curses"; }
              );

            kernelApps = lib.concatMapAttrs kernelAppsFunction kernelAppsPre;
          in
          kernelApps;

        checks = self.packages.${system};
      }
    );
}

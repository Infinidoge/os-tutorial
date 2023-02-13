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

        pkgs-i386 = (import nixpkgs {
          inherit system;
          crossSystem = {
            config = "i386-elf";
          };
        }).buildPackages;

        lib = pkgs.lib.extend (final: prev: (import ./nix/lib.nix { lib = final; }));

        callPackage = pkgs.newScope {
          inherit lib pkgs-i386;
        };


        mapKernels = drv:
          lib.mapGenAttrs
            (lib.dirsOf ./kernel)
            (callPackage drv);
      in
      {
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
            kernelPkgs = mapKernels ./nix/kernel.nix (name: {
              inherit name;
              src = ./kernel + "/${name}";
            });
          in
          kernelPkgs // rec {
            inherit (pkgs-i386) gcc binutils gdb;

            docker-image = callPackage ./nix/docker-image.nix { };
          };

        apps =
          let
            kernelAppsPre = mapKernels ./nix/run.nix (name: {
              drv = self.packages.${system}.${name};
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

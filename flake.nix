{
  description = "Subset of the os-tutorial, built with Nix";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = inputs@{ self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        lib = pkgs.lib;

        dirs = lib.filterAttrs (file: type: type == "directory") (builtins.readDir ./kernel);
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
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
                  inherit name;
                  src = ./kernel + "/${name}";
                };
              })
              dirs;
          in
          kernelPkgs;

        apps =
          let
            kernelAppsPre = lib.mapAttrs'
              (name: _: {
                inherit name;
                value = pkgs.callPackage ./kernel/run.nix {
                  drv = self.packages.${system}.${name};
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

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

            docker-image = pkgs.dockerTools.buildLayeredImage {
              name = "os-build-env";
              tag = "latest";

              contents = (with pkgs.dockerTools; [
                usrBinEnv
                binSh
              ]) ++ [
                gcc
                binutils
                gdb
              ] ++ (with pkgs; [
                bashInteractive
                coreutils-full
                diffutils
                findutils
                gawk
                gnugrep
                gnupatch
                gnused
                gnutar
                gzip
                xz
                less
                vim
                ncurses
                procps
                su
                time
                utillinux
                which
                zstd
                man
              ]);

              enableFakechroot = true;
              fakeRootCommands =
                let
                  starship-toml = pkgs.writeTextFile {
                    name = "starship.toml";
                    text = ''
                      format = """$directory
                      $character"""
                    '';
                  };

                  profile = pkgs.writeTextFile {
                    name = "bashrc";
                    text = ''
                      alias ls="ls --color=tty"
                      alias l="ls -al"

                      eval "$(${lib.getExe pkgs.starship} init bash)"

                      export STARSHIP_CONFIG="${starship-toml}"
                    '';
                  };
                in
                ''
                  #!${pkgs.runtimeShell}
                  ${pkgs.dockerTools.shadowSetup}
                  useradd --create-home user

                  cat ${profile} > /etc/bashrc
                '';


              config = rec {
                User = "user";
                WorkingDir = "/home/${User}";
                Cmd = [ "bash" ];
              };
            };
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

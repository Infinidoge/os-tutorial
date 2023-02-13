{ dockerTools
, pkgs
, lib
, pkgs-i386
}:
pkgs.dockerTools.buildLayeredImage {
  name = "os-build-env";
  tag = "latest";

  contents = (with pkgs.dockerTools; [
    # Docker stuff
    usrBinEnv
    binSh
  ]) ++ (with pkgs; [
    # Standard system tools
    bashInteractive
    coreutils-full
    diffutils
    findutils
    gawk
    gnugrep
    gnupatch
    gnused
    less
    procps
    utillinux

    # Other useful tools
    fd
    git
    moreutils
    ripgrep

    # Compress/Decompression
    gnutar
    gzip
    unzip
    zip
    zstd

    # File editing
    helix
    nano
    vim

    # Documentation
    man
    man-pages
    man-pages-posix
    tealdeer

    # Build tools/OS env requirements
    gnumake
    ncurses
  ]) ++ (with pkgs-i386; [
    # i386 toolchain
    gcc
    binutils
    gdb
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
}

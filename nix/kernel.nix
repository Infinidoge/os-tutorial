{ stdenvNoCC
, pkgs-i386
, nasm

  # Meta Attributes
, name
, src
}:
stdenvNoCC.mkDerivation {
  inherit name src;

  preferLocalBuild = true;

  nativeBuildInputs = [ pkgs-i386.gcc pkgs-i386.binutils nasm ];

  postPatch = ''
    cp ${./Makefile} Makefile
  '';

  buildPhase = ''
    make os-image.bin
    make kernel.bin
    make kernel.elf
  '';

  installPhase = ''
    mkdir -p $out
    cp os-image.bin kernel.bin kernel.elf $out
  '';
}

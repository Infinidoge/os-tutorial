{ stdenvNoCC
, gcc
, binutils
, nasm
, name
, src
}:
stdenvNoCC.mkDerivation {
  inherit name src;

  preferLocalBuild = true;

  nativeBuildInputs = [ gcc binutils nasm ];

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

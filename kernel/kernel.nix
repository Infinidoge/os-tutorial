{ stdenv, nasm, name, src }:
stdenv.mkDerivation {
  inherit name src;

  preferLocalBuild = true;

  patches = [ ./makefile.patch ];

  nativeBuildInputs = [ nasm ];

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

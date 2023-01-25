{ lib, writeScriptBin, qemu, gdb, drv }:
let
  tmp = "tmp=$(mktemp -d --tmpdir cs3502-kernel.XXXX); cd $tmp";
  i386 = "${qemu}/bin//qemu-system-i386";
  qemuFile = "-drive file=os-image.bin,index=0,if=floppy,format=raw";
in
{
  run = writeScriptBin "run" ''
    ${tmp}
    cp ${drv}/os-image.bin .
    chmod u+rwx os-image.bin
    ${i386} -vga std ${qemuFile}
  '';

  curses = writeScriptBin "curses" ''
    ${tmp}
    cp ${drv}/os-image.bin .
    chmod u+rwx os-image.bin
    ${i386} -display curses ${qemuFile}
  '';

  debug = writeScriptBin "debug" ''
    ${tmp}
    cp ${drv}/os-image.bin ${drv}/kernel.elf .
    chmod u+rwx os-image.bin
    ${i386} -display curses -s ${qemuFile}
  '';

  gdb = writeScriptBin "gdb" ''
    ${tmp}
    cp ${drv}/kernel.elf .
    cp -r ${drv.src}/kernel .
    ${lib.getExe gdb} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"
  '';
}

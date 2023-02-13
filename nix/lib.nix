{ lib }:
{
  mapGenAttrs = list: func: attrs:
    lib.genAttrs list (name: func (if builtins.typeOf attrs == "lambda" then attrs name else attrs));

  dirsOf = dir: lib.attrNames (lib.filterAttrs (file: type: type == "directory") (builtins.readDir dir));
}

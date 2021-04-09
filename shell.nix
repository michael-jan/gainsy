let
  # Pinned nixpkgs, deterministic. Last updated: 2/12/21.
  pkgs = import (fetchTarball("https://github.com/NixOS/nixpkgs/archive/a58a0b5098f0c2a389ee70eb69422a052982d990.tar.gz")) {};

  # Rolling updates, not deterministic.
  # pkgs = import (fetchTarball("channel:nixpkgs-unstable")) {};
in with pkgs; mkShell {
  nativeBuildInputs = [
    pkg-config cmake
    # scdoc
    # python3.pkgs.wrapPython
  ];

  pythonPath = [
    # python3.pkgs.colorama
  ];

  buildInputs = [
    clang pcre pcre-cpp
    ladspaH ladspa-sdk ladspaPlugins
    alsaLib alsaUtils alsaTools
    jack2 curlFull curlpp freetype
    x11 xorg.libXcomposite xorg.libXcursor xorg.libXext xorg.libXinerama
    xorg.xrandr xorg.libXrandr xorg.libXrender
    webkit mesa libGL_driver
    gtk2-x11 gtk3-x11
  ];
}

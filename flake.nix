{
    description = "Dev env for OS dev";
    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs";
    };
    outputs = inputs@{ self, nixpkgs, ... }:
    let
        system = "x86_64-linux";
        pkgs = import nixpkgs { inherit system; };
    in
    {
        devShells.${system}.default = pkgs.mkShell {
            packages = with pkgs; [
                gcc-arm-embedded
                gnumake
                qemu
                rustc
                cargo
                rust-analyzer
                rustfmt
                clang
            ];
        };
    };
}

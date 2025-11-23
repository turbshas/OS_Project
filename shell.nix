{ nixpkgs ? (import nixpkgs) {} }:
let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };
in
{
    devShells.${system}.default = pkgs.mkShell {
        packages = [
            pkgs.gcc-arm-embedded
            pkgs.gnumake
            pkgs.qemu
        ];
    };
}

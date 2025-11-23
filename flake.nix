{
    description = "Dev env for OS dev";
    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs";
    };
    outputs = inputs@{ self, nixpkgs, ... }: (import ./shell.nix) { inherit nixpkgs; };
}

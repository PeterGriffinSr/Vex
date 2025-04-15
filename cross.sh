#!/bin/bash
rm -rf cross-build
mkdir -p cross-build
mkdir -p packages

targets=(
    "aarch64-linux-gnu"  
    "i686-linux-gnu"    
    "mips64-linux-gnu"  
    "powerpc64-linux-gnu"  
    "x86_64-w64-mingw32"
    "arm-linux-gnu"      
    "i686-w64-mingw32"  
    "mips-linux-gnu" 
    "x86_64-linux-gnu"
)

cross_dir="crossfiles"

package_and_hash() {
    local target=$1
    local build_dir="cross-build/$target"
    local package_dir="packages"
    local package_name="${package_dir}/${target}.tar.gz"

    mkdir -p "$package_dir"

    local binary_path
    binary_path=$(find "$build_dir" -type f \( -name "vex" -o -name "vex.exe" \) -print -quit)

    tar -czf "$package_name" -C "$(dirname "$binary_path")" "$(basename "$binary_path")"

    local hash_file="${package_dir}/${target}-hashes.txt"
    sha256sum "$binary_path" > "$hash_file"
}

native_build_dir="cross-build/default-build"

meson setup "$native_build_dir"
meson compile -C "$native_build_dir" -j"$(nproc)"

package_and_hash "default-build"

for target in "${targets[@]}"; do
    build_dir="cross-build/$target"
    mkdir -p "$build_dir"

    meson setup "$build_dir" . --cross-file "$cross_dir/$target.txt"
    meson compile -C "$build_dir" -j"$(nproc)"

    package_and_hash "$target"
done

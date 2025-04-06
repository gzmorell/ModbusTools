alias b := build
alias r := run
alias g := generate
alias c := commit
alias m := main
alias w := write
alias p := pull
alias rb := rebase

default_type := 'Debug'

distro := if os_family() == "unix" {`rg -N '^ID=(.*)' -or '$1' '/etc/os-release'`} else {"windows"}
exec_path := if os_family() == "unix" { "./build/" } else { "./build" }
security_path := if os_family() == "unix" { "./build/security" } else { "./build" }
cmake_base_options := "-DCMAKE_VERBOSE_MAKEFILE=TRUE -DCMAKE_EXPORT_COMPILE_COMMANDS=1"
cmake_options := if os_family() == "windows" { '-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ' + cmake_base_options } else { if distro == 'ubuntu' {'-DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake ' + cmake_base_options } else {cmake_base_options}}
isubuntu := if distro == 'ubuntu' {'yes'} else {'no'}

# List options
default:
    just --list --unsorted

oficina:
    sed -i 's/\"127\.0\.0\.1\"/\"192\.168\.1\.156\"/g' ./config/settings.json
local:
    sed -i 's/\"192\.168\.1\.156\"/\"127\.0\.0\.1\"/g' ./config/settings.json

sync:
    cp gateway/config/parameters.json config/parameters.json
    cp gateway/config/commands.json config/commands.json
    cp config/settings.json gateway/config/settings.json
    cp config/settings.json security/config/settings.json

info:
    @echo "{{arch()}}-{{os()}}-{{distro}}"
    @echo "{{cmake_options}}"

# Git Pull
pull:
    git pull

# Git Checkout main
main:
    git checkout main
    git pull

# Git Checkout write
write:
    git checkout write
    git pull

# Git Commit and push all changes with a message
commit message: pull
    git add .
    git commit -m "{{message}}"
    git push

# Git Rebase write in main and push
rebase: main
    git rebase write
    git push
    git checkout write

# Generate Ninja Multi-Config
generate: 
    cmake -G "Ninja Multi-Config" {{cmake_options}} -B build -S .

# Generate and Build all - Debug/Release, default Debug
build type=default_type: generate
    cmake --build build --config {{type}} --target all -j2
    cp build/compile_commands.json .

# Generate, Build and Run gateway Debug/Release, default Debug
run program type=default_type: (build type)
    {{exec_path}}/{{type}}/{{program}}

publish: pull (build "Release")
    scp build/Release/gateway cm4:

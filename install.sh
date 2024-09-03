# check if the user has chosen a compiler
if [ -z "$1" ]; then
    echo "No compiler chosen, defaulting to gcc"
    COMPILER="gcc"
else
    COMPILER=$1
fi

# download the latest version of mdpu.c
curl -O https://raw.githubusercontent.com/zanderlewis/mdpu.c/main/mdpu.c

# compile the mdpu.c file with the user's chosen compiler
$COMPILER mdpu.c -o mdpu

echo "mdpu has been installed successfully. You can run it with ./mdpu <registers> <memory> <filename>"

# remove the mdpu.c file
rm mdpu.c

# Example run from github:
# curl -O https://raw.githubusercontent.com/zanderlewis/mdpu.c/main/install.sh
# chmod +x install.sh
# ./install.sh gcc
# ./mdpu 18 10x10x2 0.instr
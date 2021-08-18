#!/usr/bin/env sh

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color
CURSOR_UP="\033[1A"
CURSOR_CLEAR="\033[2K"
CURSOR_RESET="\033[0J"

check_command() {
  command -v $1 >/dev/null 2>&1 || { echo >&2 "${RED}$1 not found! Please install!${NC}"; exit 1; }
}

echo "Checking dependencies..."

check_command "cmake"
check_command "ninja"
check_command "git"

sleep 1
echo "${CURSOR_UP}${CURSOR_CLEAR}${CURSOR_RESET}${GREEN}All dependencies found!${NC}"

check_file_exists() {
  if [ ! -f $1 ]; then
    return 1;
  fi
  return 0;
}

NAME="blossom_$(date +%Y-%m)"

if check_file_exists $HOME/.blname; then
  NAME=`cat $HOME/.blname`
  rm -rf $NAME
else
  touch $HOME/.blname
  echo "$NAME" > $HOME/.blname
fi

git clone https://github.com/cstanze/blossom.git $NAME

cd $NAME

mkdir build

cd build

cmake -DCMAKE_BUILD_TYPE=Release -G Ninja ..

CORES=`python -c 'import multiprocessing as mp; print(mp.cpu_count())'`

ninja install -j$CORES

echo "${GREEN}Blossom finished installing!${NC}"

echo "Installed to: ${HOME}/.blossom"

yes_or_no() {
  while true; do
    read -p "$1 [y/n] " yn
    case $yn in
      [Yy]* ) return 0;;
      [Nn]* ) return 1;;
      * ) echo "Please answer yes (y) or no (n).";;
    esac
  done
}

yes_or_no "Do you want to add blossom to your PATH?" || exit

CONFIG_PATH=""

if [ -f ~/.bashrc ]; then
  CONFIG_PATH="$HOME/.bashrc"
elif [ -f ~/.zshrc ]; then
  CONFIG_PATH="$HOME/.zshrc"
elif [ -f ~/.profile ]; then
  CONFIG_PATH="$HOME/.profile"
elif [ -f ~/.bash_profile ]; then
  CONFIG_PATH="$HOME/.bash_profile"
elif [ -f ~/.bash_login ]; then
  CONFIG_PATH="$HOME/.bash_login"
fi

echo "Adding to your \$PATH..."
echo "export PATH=\"\$PATH:$HOME/.blossom/bin\"" >> $CONFIG_PATH

cd -

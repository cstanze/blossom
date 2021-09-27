#!/usr/bin/env bash

# This script will install the compiler files and libraries from the
# tarball (which is assumed to be the current directory).

# Prompt the user for the directory to install to
echo "Where would you like to install the compiler?"
read -e -p "Installation directory: " INSTALL_DIR

# Make the directory if it doesn't exist
mkdir -p "$INSTALL_DIR"
if [ ! -d "$INSTALL_DIR" ]; then
  echo "Error: Unable to create directory $INSTALL_DIR"
  exit 1
fi

# Install the compiler
echo "Installing compiler..."

cp -r . $INSTALL_DIR
rm -rf $INSTALL_DIR/install.sh # Remove the install script, since it's no longer needed

echo "Done."

echo "Compiler installed to $INSTALL_DIR"

add_to_path() {
  if [ -z "$1" ]; then
    echo "Error: No config path provided"
    exit 1
  fi

  if [ -z "$2" ]; then
    echo "Error: No path provided"
    exit 1
  fi

  read -n 1 -p "Add compiler to PATH? [y/n] "
  if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "export PATH=$1:\$PATH" >> $1
  fi
}

if [ -f "$HOME/.zshrc" ]; then
  add_to_path "$HOME/.zshrc" "$INSTALL_DIR/bin"
elif [ -f "$HOME/.bashrc" ]; then
  add_to_path "$HOME/.bashrc" "$INSTALL_DIR/bin"
elif [ -f "$HOME/.profile" ]; then
  add_to_path "$HOME/.profile" "$INSTALL_DIR/bin"
elif [ -f "$HOME/.bash_profile" ]; then
  add_to_path "$HOME/.bash_profile" "$INSTALL_DIR/bin"
fi

echo "\nThanks for using Blossom! Happy Coding!"

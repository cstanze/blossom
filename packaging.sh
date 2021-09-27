#!/usr/bin/env bash

# This script is used to package the compiler for distribution.
#
# The output is a distributable tarball that contains the compiler binaries,
# libraries, and installation scripts.
#
# Usage:
# ./packaging.sh <source_dir> <build_dir> <output_dir> <resources_dir>
#
# Arguments:
#   build_dir: The directory in which the compiler was built.
#   output_dir: The directory in which to write the output.


# Make sure the script is not being executed with superuser privileges.
if [[ $EUID -eq 0 ]]; then
  echo "It is advisable to run this script without superuser privileges."
  exit 1
fi

USAGE="Usage: $0 <source_dir> <build_dir> <output_dir> <resources_dir>"

# Make sure the script is invoked with at least three arguments.
if [[ $# -lt 4 ]]; then
  echo $USAGE
  exit 1
fi

# If -h or --help is available anywhere on the command line, print the usage
# message and exit.
if [[ "$@" =~ "--help" ]] || [[ "$@" =~ "-h" ]]; then
  echo $USAGE
  exit 0
fi

# Get the build directory and output directory.
SOURCE_DIR="$1"
BUILD_DIR="$2"
OUTPUT_DIR="$3"
RESOURCES_DIR="$4"

# Make sure all of the required arguments are valid.
if [[ ! -d "$SOURCE_DIR" ]]; then
  echo "The source directory does not exist: ${SOURCE_DIR}"
  exit 1
fi
if [[ ! -d "${BUILD_DIR}" ]]; then
  echo "The build directory does not exist: ${BUILD_DIR}"
  exit 1
fi
if [[ ! -d "${OUTPUT_DIR}" ]]; then
  mkdir -p "${OUTPUT_DIR}"
  if [[ $? -ne 0 ]]; then
    echo "The output directory could not be created: ${OUTPUT_DIR}"
    exit 1
  fi
fi
if [[ ! -d "${RESOURCES_DIR}" ]]; then
  echo "The resources directory does not exist: ${RESOURCES_DIR}"
  exit 1
fi


# Get a release name that
# starts with Blossom_ followed by a semver version number optionally followed by a dash and a pre-release identifier.
# (e.g. Blossom_1.0.0-alpha.1, Blossom_1.0.1-RC.1, Blossom_1.0.1)
# if the release name is not valid, show an error message and loop until a valid release name is entered.
while [[ ! "${RELEASE_NAME}" =~ ^Blossom_(osx|win|nix)_[0-9]+\.[0-9]+\.[0-9]+(-[a-zA-Z0-9]+\.[0-9]+)?$ ]]; do
  read -p "Release name: " RELEASE_NAME
  # If not valid, show an error
  if [[ ! "${RELEASE_NAME}" =~ ^Blossom_(osx|win|nix)_[0-9]+\.[0-9]+\.[0-9]+(-[a-zA-Z0-9]+\.[0-9]+)?$ ]]; then
    echo "Invalid release name (e.g. Blossom_osx_0.1.0-alpha.1, Blossom_nix_0.1.0-RC.1, Blossom_win_0.1.0)"
  fi
done

# Create a temporary directory to store the output.
TEMP_DIR=$(mktemp -d)

# Move subdirs into temp dir
cp -r "${BUILD_DIR}/bin" "${TEMP_DIR}"
cp -r "${BUILD_DIR}/lib" "${TEMP_DIR}"
cp -r "${SOURCE_DIR}/cmake" "${TEMP_DIR}"
cp -r "${SOURCE_DIR}/include" "${TEMP_DIR}"

if [[ -x "$(command -v json_pp)" ]]; then
  # Use json_pp to pretty-print the JSON files.
  echo "{ \"includePaths\": [], \"dllPaths\": [] }" | json_pp > "${TEMP_DIR}/config.json"
else
  echo "{ \"includePaths\": [], \"dllPaths\": [] }" > "${TEMP_DIR}/config.json"
fi

# Find install.sh in resources dir exit if not found
INSTALL_SH="${RESOURCES_DIR}/install.sh"
if [[ ! -f "${INSTALL_SH}" ]]; then
  echo "install.sh not found in ${RESOURCES_DIR}"
  exit 1
fi

# Copy install.sh to temp dir
cp "${INSTALL_SH}" "${TEMP_DIR}"

# Create a tarball from the temporary directory.
tar -czf "${OUTPUT_DIR}/${RELEASE_NAME}.tar.gz" -C "${TEMP_DIR}" .

# Remove the temporary directory and its contents.
rm -rf "${TEMP_DIR}"

# Show the release name and the path to the tarball.
echo "Created ${RELEASE_NAME}.tar.gz in ${OUTPUT_DIR}"

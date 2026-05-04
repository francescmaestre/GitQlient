#!/bin/bash

if [[ ! `uname` == "Darwin"* ]];
then
    echo "This script is designed to be run by macOS devices only"
    exit 1
fi

if (( $# < 1 ));
then
    echo "Usage: ${0} path-to-app (optional)output-path (optional)path-to-resources"
    exit 1
fi

APP_PATH=${1}
# exists and is in the format we expect
if [ ! -e "${APP_PATH}" ] || [[ ! "${APP_PATH}" == *".app" ]];
then
    echo "Unrecognised app path: ${APP_PATH}"
    exit 1
fi
APP_NAME="${APP_PATH##*/}"
APP_NAME="${APP_NAME%*.app}"

if (( $# > 1 ));
then
    OUTPUT_DIR=${2}
else
    if [[ "${APP_PATH}" == *"/"* ]];
    then
        OUTPUT_DIR=${APP_PATH%/*}
    fi

    if [ -z "${OUTPUT_DIR}" ]; then
        OUTPUT_DIR=`pwd`
    fi
fi

# is a dir that we can write to
if [ ! -d "${OUTPUT_DIR}" ] || [ ! -w "${OUTPUT_DIR}" ];
then
    echo "Detected output path is not a directory or this script does not have write permissions: ${OUTPUT_DIR}"
    exit 1
fi
DMG_PATH="${OUTPUT_DIR}/${APP_NAME}.dmg"

if (( $# > 2 ));
then
    RESOURCES_DIR=${3}
else
    RESOURCES_DIR=$(dirname $(realpath ${0}))/../src/resources
fi

# is a dir that we can read from
if [ ! -d "${RESOURCES_DIR}" ];
then
    echo "Couldn't find the resources dir. Tried: ${RESOURCES_DIR}"
    exit 1
fi

# Check for create-dmg on PATH, else try and look for it in a homebrew install
CREATE_DMG_EXE=`command -v create-dmg`
if [ -z "${CREATE_DMG_EXE}" ];
then
    CREATE_DMG_EXE="/opt/homebrew/bin/create-dmg"
    if [ ! -x "${CREATE_DMG_EXE}" ];
    then
        echo "Couldn't find \"create-dmg\", is it installed? Try: brew install create-dmg"
        exit 1
    fi
fi

echo "########################"
echo "# Generating DMG using:"
echo "#   CREATE_DMG_EXE: ${CREATE_DMG_EXE}"
echo "#   APP_PATH: ${APP_PATH}"
echo "#   RESOURCES: ${RESOURCES_DIR}"
echo "#   OUTPUT: ${DMG_PATH}"
echo "########################"

if [ -e "${DMG_PATH}" ]; then
    rm ${DMG_PATH}
fi

${CREATE_DMG_EXE} \
  --volname ${APP_NAME} \
  --volicon ${RESOURCES_DIR}/icon.icns \
  --background ${RESOURCES_DIR}/dmg_bg.png \
  --icon ${APP_NAME}.app 125 220 \
  --icon-size 100 \
  --window-size 600 450 \
  --app-drop-link 475 220 ${DMG_PATH} ${APP_PATH}

FILEICON_EXE=`command -v fileicon`
if [ -z "${FILEICON_EXE}" ]; then
    FILEICON_EXE="/opt/homebrew/bin/fileicon"
fi

if [ -x "${FILEICON_EXE}" ]; then
    ${FILEICON_EXE} set "${DMG_PATH}" "${RESOURCES_DIR}/icon.icns"
else
    echo "Warning: fileicon not found, DMG file icon not set. Try: brew install fileicon"
fi

exit 0

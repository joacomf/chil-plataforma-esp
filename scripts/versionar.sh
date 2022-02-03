#!/bin/bash

BASEDIR=$(dirname "$0")
ARCHIVO_VERSION="$BASEDIR/../version"
ARCHIVO_LIBRARY="$BASEDIR/../library.json"

obtenerVersion() {
    cat "$ARCHIVO_VERSION"
}

verificarCorrectitudDeComando() {
    if [ -z "$1" ]; then
        echo -e "\e[91mError:\e[39m"
        echo "Se debe indicar que se parte de la versión se desea incrementar"
        echo "Opciones:"
        echo -e "    \e[1mmayor\e[0m (o \e[1mM\e[0m)"
        echo -e "    \e[1mminor\e[0m (o \e[1mm\e[0m)"
        echo -e "    \e[1mpatch\e[0m (o \e[1mp\e[0m)"
        exit 1;
    fi
}

aumentarVersion() {
    VERSION_ACTUAL=$(obtenerVersion)

    IFS='.' read -r -a numerosVersion <<< "$VERSION_ACTUAL"

    MAYOR="${numerosVersion[0]}"
    MINOR="${numerosVersion[1]}"
    PATCH="${numerosVersion[2]}"

    echo "La versión actual es $MAYOR.$MINOR.$PATCH"

    if [[ $1 == "mayor" ]] || [[ $1 == "M" ]]; then
        MAYOR="$(("$MAYOR" + 1))"
        MINOR=0
        PATCH=0
    fi

    if [[ $1 == "minor" ]] || [[ $1 == "m" ]]; then
        MINOR="$(("$MINOR" + 1))"
        PATCH=0
    fi

    if [[ $1 == "patch" ]] || [[ $1 == "p" ]]; then
        PATCH="$(("$PATCH" + 1))"
    fi

    NUEVA_VERSION="$MAYOR.$MINOR.$PATCH"

    echo "La nueva versión es $NUEVA_VERSION"
    echo "$NUEVA_VERSION" > "$ARCHIVO_VERSION"
}

cambiarVersionEnLibraryJSON() {
    VERSION=$(obtenerVersion)
    jq '.version = $version' --arg version "$VERSION" "$ARCHIVO_LIBRARY" > tmp.$$.json && mv tmp.$$.json "$ARCHIVO_LIBRARY"
}


verificarCorrectitudDeComando "$1"
aumentarVersion "$1"
cambiarVersionEnLibraryJSON


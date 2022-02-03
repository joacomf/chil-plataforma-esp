BASEDIR=$(dirname "$0")
MENSAJE_COMMIT=$1

obtenerTipoVersion() {
    TIPO_VERSION='patch'
    if [[ $1 =~ ^\[MAYOR\].* ]]; then
        TIPO_VERSION='mayor'
    elif [[ $1 =~ ^\[MINOR\].* ]]; then
        TIPO_VERSION='minor'
    fi

    echo "$TIPO_VERSION"
}

echo -e "\e[33mAumentando versión...\e[39m"
TIPO="$(obtenerTipoVersion "$MENSAJE_COMMIT")"
"$BASEDIR"/versionar.sh "$TIPO"
echo -e "\xE2\x9C\x94 Listo"

VERSION=$(cat "$BASEDIR/../version")

echo -e "\e[33mEstado de git\e[39m"
git status

echo -e  "\e[33mConfigurando usuario Pipeline\e[39m"
git config user.name "CD Pipeline"
git config user.email "<>"
echo -e "\xE2\x9C\x94 Listo"

echo -e "\e[33mGenerando tag con version ${VERSION}\e[39m"
git add version library.json
git commit -m "[VERSION] Genera version ${VERSION}"
git tag -m "${VERSION}" "${VERSION}"
git push
git push --tags
echo -e "\xE2\x9C\x94 Subido"

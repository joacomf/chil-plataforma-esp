#!/bin/bash

set -e

echo -e "\e[33mGenerando paquete\e[39m"
pio package pack .
echo -e "\xE2\x9C\x94 Listo"

echo -e "\e[33mListando directorio\e[39m"
ls -l
echo -e "\xE2\x9C\x94 Listo"

echo -e "\e[33mPublicando paquete\e[39m"
pio package publish --non-interactive
echo -e "\xE2\x9C\x94 Listo"


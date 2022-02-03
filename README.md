# Plataforma Arduino para Chil

Este modulo tiene la intención de ser el adaptador con funciones específicas de Arduino para que se pueda utilizar en Chil. Esta separación surge para abstraer el framework de pruebas de tener librerías específicas de las plataformas.

Como contrato esta implementación toma la interfaz de `Framework` contenida en el proyecto base. Para poder utilizar estoy y que las pruebas descritas este proyecto pueda ser corridas aisladas en un dispositivo se creó la librería `Plataforma` en el directorio `lib` para poder referenciar a la interfaz original y que el compilador no falle en encontrar el archivo.

## Dependencias

Como librería para tests se utilizó [AUnit](https://github.com/bxparks/AUnit).

Para correr fácilmente las pruebas en un dispositivo es necesario tener instalada la herramienta de línea de comando de `platformio` del paquete de PlatforIO Core.

Siguiendo los [pasos de la documentación oficial](https://docs.platformio.org/en/latest/core/installation.html#installation-methods) se puede instalar fácilmente.


Teniendo instalada la herramienta pueden correr los tests de dos maneras:
1. Ejecutando el script por consola
```
./correr.sh
```
Este script tiene la ventaja que detecta cuando terminan de ejecutarse los tests y sale de su ejecución.

2. Ejecutando 
```
platformio test -e esp32dev 
```
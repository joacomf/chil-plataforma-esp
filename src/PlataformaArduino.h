#ifndef CHIL_FRAMEWORKARDUINO_H
#define CHIL_FRAMEWORKARDUINO_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

#include <Plataforma.h>
#include <PuntoDeEntrada.h>

#define PLATAFORMA_ARDUINO new PlataformaArduino()


class PlataformaArduino: public Plataforma {
private:
    int velocidadSerial = 115200;
    bool apEncendido = false;
    bool servidorCorriendo = false;
    static const byte DNS_PORT = 53;

    AsyncWebServer* servidor{};

    IPAddress ipLocal = IPAddress(192, 168, 4, 1);
    IPAddress sinIPDeclarada = INADDR_NONE;
    IPAddress dns = IPAddress(192, 168, 4, 254);

    [[noreturn]] static void configurarServidorDNS(void *parametros);

public:
    PlataformaArduino();
    void consola(const char* text) override;
    void escribir(int pin, int valor) override;
    int leer(int pin) override;
    void demorar(int milisegundos) override;
    unsigned long milisegundos() override;
    unsigned long microsegundos() override;
    void pinEntrada(int pin) override;
    void pinSalida(int pin) override;
    bool crearRedWiFi(const char *nombre, const char *clave) override;
    bool estaAPEncendido() override;
    bool apagarWiFi() override;

    void crearServidorWeb() override;

    void configurarPuntoDeEntrada(PuntoDeEntrada* puntoDeEntrada) override;
    bool estaServidorCorriendo() override;
    void eliminarServidorWeb() override;

    void configurarMockUrls() override;
    void eliminarMocksUrls() override;
};
#endif //CHIL_FRAMEWORKARDUINO_H

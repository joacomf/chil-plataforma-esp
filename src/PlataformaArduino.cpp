#include "PlataformaArduino.h"

PlataformaArduino::PlataformaArduino() {
    Serial.begin(PlataformaArduino::velocidadSerial);
}

void PlataformaArduino::escribir(int pin, int valor) {
    digitalWrite(pin, valor);
}

int PlataformaArduino::leer(int pin) {
    return digitalRead(pin);
}

unsigned long PlataformaArduino::milisegundos() {
    return millis();
}

unsigned long PlataformaArduino::microsegundos() {
    return micros();
}

void PlataformaArduino::demorar(int milisegundos) {
    delay(milisegundos);
}

void PlataformaArduino::pinEntrada(int pin) {
    pinMode(pin, INPUT);
}

void PlataformaArduino::pinSalida(int pin) {
    pinMode(pin, OUTPUT);
}

void PlataformaArduino::consola(const char *texto) {
    Serial.println(texto);
}

bool PlataformaArduino::crearRedWiFi(const char *nombre, const char *clave) {
    this->apEncendido = WiFi.softAP(nombre, clave);

    if (!WiFi.config(this->ipLocal, this->sinIPDeclarada, this->sinIPDeclarada, this->ipLocal, INADDR_NONE)) {
        this->consola("Fallo en configurar WiFi");
    }

    return this->apEncendido;
}

bool PlataformaArduino::estaAPEncendido() {
    return this->apEncendido;
}

bool PlataformaArduino::apagarWiFi() {
    this->apEncendido = WiFi.softAPdisconnect(true);
    return this->apEncendido;
}

void PlataformaArduino::eliminarServidorWeb() {
    if (this->servidorCorriendo) {
        this->servidor->end();
        this->servidorCorriendo = false;
    }
}

void PlataformaArduino::crearServidorWeb() {
    if (!this->servidorCorriendo) {
        this->servidor = new AsyncWebServer(80);
        this->servidor->on("/chil-ping", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(200, "text/plain", "chil-pong");
        });

        this->servidor->begin();
        this->servidorCorriendo = true;
    }
}

bool PlataformaArduino::estaServidorCorriendo() {
    return this->servidorCorriendo;
}

void PlataformaArduino::configurarPuntoDeEntrada(PuntoDeEntrada *puntoDeEntrada) {
    WebRequestMethod metodoAConfigurar = HTTP_GET;

    if (puntoDeEntrada->obtenerMetodo() == POST) {
        metodoAConfigurar = HTTP_POST;
    }

    this->servidor->reset();
    this->servidor->on(puntoDeEntrada->obtenerRuta(), metodoAConfigurar,
                       [puntoDeEntrada](AsyncWebServerRequest *request) {
                           delay(10);
                           request->send(puntoDeEntrada->obtenerCodigoDeRespuesta(), puntoDeEntrada->obtenerTipo(), puntoDeEntrada->obtenerRespuesta());
                       });
}

TaskHandle_t manejadorTareaDeConfiguracionServidorDNS = nullptr;
void PlataformaArduino::configurarMockUrls() {
    xTaskCreate(
            PlataformaArduino::configurarServidorDNS,
            "configuraDNS",
            10000,
            nullptr,
            1,
            &manejadorTareaDeConfiguracionServidorDNS);
}

[[noreturn]] void PlataformaArduino::configurarServidorDNS(void *parametros) {
    DNSServer dnsServer;
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(PlataformaArduino::DNS_PORT, "*", WiFi.softAPIP());

    while (true) {
        delay(10);
        dnsServer.processNextRequest();
    }
}

void PlataformaArduino::eliminarMocksUrls() {
    if(manejadorTareaDeConfiguracionServidorDNS != nullptr) {
        vTaskDelete(manejadorTareaDeConfiguracionServidorDNS);
    }
}
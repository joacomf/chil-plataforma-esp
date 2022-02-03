#include "PlataformaESP.h"

PlataformaESP::PlataformaESP() {
    Serial.begin(PlataformaESP::velocidadSerial);
}

void PlataformaESP::escribir(int pin, int valor) {
    digitalWrite(pin, valor);
}

int PlataformaESP::leer(int pin) {
    return digitalRead(pin);
}

unsigned long PlataformaESP::milisegundos() {
    return millis();
}

unsigned long PlataformaESP::microsegundos() {
    return micros();
}

void PlataformaESP::demorar(int milisegundos) {
    delay(milisegundos);
}

void PlataformaESP::pinEntrada(int pin) {
    pinMode(pin, INPUT);
}

void PlataformaESP::pinSalida(int pin) {
    pinMode(pin, OUTPUT);
}

void PlataformaESP::consola(const char *texto) {
    Serial.println(texto);
}

bool PlataformaESP::crearRedWiFi(const char *nombre, const char *clave) {
    this->apEncendido = WiFi.softAP(nombre, clave);

    if (!WiFi.config(this->ipLocal, this->sinIPDeclarada, this->sinIPDeclarada, this->ipLocal, INADDR_NONE)) {
        this->consola("Fallo en configurar WiFi");
    }

    return this->apEncendido;
}

bool PlataformaESP::estaAPEncendido() {
    return this->apEncendido;
}

bool PlataformaESP::apagarWiFi() {
    this->apEncendido = WiFi.softAPdisconnect(true);
    return this->apEncendido;
}

void PlataformaESP::eliminarServidorWeb() {
    if (this->servidorCorriendo) {
        this->servidor->end();
        this->servidorCorriendo = false;
    }
}

void PlataformaESP::crearServidorWeb() {
    if (!this->servidorCorriendo) {
        this->servidor = new AsyncWebServer(80);
        this->servidor->on("/chil-ping", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(200, "text/plain", "chil-pong");
        });

        this->servidor->begin();
        this->servidorCorriendo = true;
    }
}

bool PlataformaESP::estaServidorCorriendo() {
    return this->servidorCorriendo;
}

void PlataformaESP::configurarPuntoDeEntrada(PuntoDeEntrada *puntoDeEntrada) {
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
void PlataformaESP::configurarMockUrls() {
    xTaskCreate(
            PlataformaESP::configurarServidorDNS,
            "configuraDNS",
            10000,
            nullptr,
            1,
            &manejadorTareaDeConfiguracionServidorDNS);
}

[[noreturn]] void PlataformaESP::configurarServidorDNS(void *parametros) {
    DNSServer dnsServer;
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(PlataformaESP::DNS_PORT, "*", WiFi.softAPIP());

    while (true) {
        delay(10);
        dnsServer.processNextRequest();
    }
}

void PlataformaESP::eliminarMocksUrls() {
    if(manejadorTareaDeConfiguracionServidorDNS != nullptr) {
        vTaskDelete(manejadorTareaDeConfiguracionServidorDNS);
    }
}
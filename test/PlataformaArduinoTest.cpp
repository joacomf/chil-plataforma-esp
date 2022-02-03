#include "../src/PlataformaArduino.cpp"
#include "../../interfaces/PuntoDeEntrada.cpp"
#include <AUnit.h>
#include <utility>

using namespace aunit;

PlataformaArduino *framework;
int led = 2;

void setup() {
    delay(1000);
    framework = new PlataformaArduino();
}

test(EntradaSalidaDigital, deberiaEncenderElLedIndicado) {
    framework->pinSalida(led);
    framework->escribir(led, HIGH);

    assertEqual(framework->leer(led), HIGH);
}

test(Tiempo, deberiaObtenerElTiempoEnMicrosegundos) {
    unsigned long cero = 0;

    unsigned long primeraMedicion = framework->microsegundos();
    unsigned long segundaMedicion = framework->microsegundos();

    assertMore(primeraMedicion, cero);
    framework->demorar(1);
    assertMoreOrEqual(segundaMedicion, primeraMedicion);
}

test(Tiempo, deberiaObtenerElTiempoUtilizandoMilisegundos) {
    unsigned long cero = 0;

    unsigned long primeraMedicion = framework->milisegundos();
    framework->demorar(10);
    unsigned long segundaMedicion = framework->milisegundos();

    assertMore(primeraMedicion, cero);
    assertMore(segundaMedicion, primeraMedicion);
}

test(Tiempo, deberiaDemorar100milisegundosEntreLasMediciones) {
    long tiempoDeDemora = 100;

    unsigned long primeraMedicion = framework->milisegundos();
    framework->demorar(tiempoDeDemora);
    unsigned long segundaMedicion = framework->milisegundos();

    unsigned long diferenciaDeTiempo = segundaMedicion - primeraMedicion;

    assertMoreOrEqual(diferenciaDeTiempo, (unsigned long) tiempoDeDemora);
}

test(Consola, deberiaImprimirPorConsola) {
    framework->consola("Verificacion manual: Deberia imprimir este texto por consola");
}

test(WiFi, deberiaEstarEncendidaLaRedWiFiAPLuegoDeCrearla) {
    framework->crearRedWiFi("hola", "mundo12345");
    assertTrue(framework->estaAPEncendido());
    framework->apagarWiFi();
}

test(WiFi, deberiaEstarApagadoLaRedWiFIAPSiNuncaSeEncendio) {
    framework->apagarWiFi();
    framework->demorar(400);

    assertFalse(framework->estaAPEncendido());
}

static String dispositivoBaseUrl = "http://192.168.4.1";

class Respuesta {
public:
    int codigo;
    String contenido;

    Respuesta(int codigo, String contenido) {
        this->codigo = codigo;
        this->contenido = std::move(contenido);
    }
};

class ServidorWebTest : public TestOnce {
protected:
    void setup() override {
        TestOnce::setup();
        framework->crearRedWiFi("servidor", "http12345");
        framework->crearServidorWeb();
        framework->demorar(10);
    }

    void teardown() override {
        framework->apagarWiFi();
        framework->eliminarServidorWeb();
        TestOnce::teardown();
    }

    static Respuesta *alHacerPeticionGET(const char *camino) {
        HTTPClient cliente;
        cliente.begin(dispositivoBaseUrl + camino);

        int codigoDeRespuesta = cliente.GET();
        String contenido = cliente.getString();
        cliente.end();

        return new Respuesta(codigoDeRespuesta, contenido);
    }

    static Respuesta *alHacerPeticionPOST(const char *camino) {
        HTTPClient cliente;
        cliente.begin(dispositivoBaseUrl + camino);

        int codigoDeRespuesta = cliente.POST("");
        cliente.end();

        return new Respuesta(codigoDeRespuesta, "");
    }

    static Respuesta *alHacerPeticionGETAUrl(const char *url) {
        HTTPClient cliente;
        cliente.begin(url);

        int codigoDeRespuesta = cliente.GET();
        String contenido = cliente.getString();
        cliente.end();

        return new Respuesta(codigoDeRespuesta, contenido);
    }
};

test(ServidorWebTest, deberiaNoTenerServidorWebCreadoPorDefault) {
    assertFalse(framework->estaServidorCorriendo());
}

testF(ServidorWebTest, deberiaCrearUnServidorWeb) {
    assertTrue(framework->estaServidorCorriendo());
}

testF(ServidorWebTest, deberiaEliminarServidorWebLuegoDeEncenderlo) {
    framework->eliminarServidorWeb();

    assertFalse(framework->estaServidorCorriendo());
}

testF(ServidorWebTest, deberiaCrearUnPuntoDeEntradaDePingAlCrearElServidorWeb) {
    Respuesta *respuesta = alHacerPeticionGET("/chil-ping");

    assertEqual(200, respuesta->codigo);
    assertEqual("chil-pong", respuesta->contenido);
}

testF(ServidorWebTest, deberiaAgregarPuntoDeEntradaAlServidorCreado) {
    auto *puntoDeEntrada = new PuntoDeEntrada("/numeros");
    puntoDeEntrada->configurarRespuesta("numeros", "text/plain", 200);

    framework->configurarPuntoDeEntrada(puntoDeEntrada);

    Respuesta *respuesta = alHacerPeticionGET("/numeros");

    assertEqual(200, respuesta->codigo);
    assertEqual("numeros", respuesta->contenido);
}

testF(ServidorWebTest, deberiaAgregarPuntoDeEntradaParaMetodoPostAlServidorCreado) {
    auto *puntoDeEntrada = new PuntoDeEntrada("/ping", POST);
    puntoDeEntrada->configurarRespuesta("", "application/json", 200);

    framework->configurarPuntoDeEntrada(puntoDeEntrada);

    Respuesta *respuesta = alHacerPeticionPOST("/ping");

    assertEqual(200, respuesta->codigo);
}

testF(ServidorWebTest, deberiaResponderElServidorLocalSiSeMockeaLaUrl) {
    framework->configurarMockUrls();
    framework->demorar(100);

    auto *puntoDeEntrada = new PuntoDeEntrada("/", GET);
    puntoDeEntrada->configurarRespuesta("Esto no es Google", "text/plain", 202);
    framework->configurarPuntoDeEntrada(puntoDeEntrada);

    Respuesta *respuesta = alHacerPeticionGETAUrl("http://www.google.com/");

    framework->eliminarMocksUrls();
    assertEqual(202, respuesta->codigo);
    assertEqual("Esto no es Google", respuesta->contenido);
}

void loop() {
    TestRunner::run();
}
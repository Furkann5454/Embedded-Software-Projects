
#include <Arduino.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define DHT_PIN D2
#define DHT_TYPE DHT11
#define SOIL_PIN A0

DHT dht(DHT_PIN, DHT_TYPE);
ESP8266WebServer server(80);

// Sensor verileri
float sicaklik = NAN;
float havaNem = NAN;
int toprakNem = 0;

unsigned long sonOlcum = 0;
const unsigned long olcumAraligi = 2000;

// Web sayfamiz
const char WEB_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="tr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport"
          content="width=device-width, initial-scale=1.0">

    <title>Smart Plant Care</title>

    <style>
        * {
            box-sizing: border-box;
        }

        body {
            margin: 0;
            padding: 25px;
            font-family: Arial, sans-serif;
            background: #101827;
            color: #f8fafc;
        }

        .container {
            max-width: 1000px;
            margin: 40px auto;
        }

        h1 {
            font-size: 32px;
            margin-bottom: 10px;
        }

        .subtitle {
            color: #94a3b8;
            margin-bottom: 30px;
        }

        .status {
            display: inline-block;
            padding: 10px 16px;
            background: #153c30;
            color: #4ade80;
            border-radius: 20px;
            margin-bottom: 30px;
        }

        .grid {
            display: grid;
            grid-template-columns:
                repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
        }

        .card {
            background: #1e293b;
            padding: 28px;
            border-radius: 18px;
            border: 1px solid #334155;
        }

        .card h3 {
            color: #94a3b8;
            font-size: 15px;
            font-weight: normal;
        }

        .value {
            font-size: 38px;
            font-weight: bold;
            margin-top: 20px;
        }

        .temperature {
            color: #fb923c;
        }

        .humidity {
            color: #38bdf8;
        }

        .soil {
            color: #4ade80;
        }

        .footer {
            margin-top: 30px;
            color: #64748b;
            font-size: 13px;
        }
    </style>
</head>

<body>
    <div class="container">

        <h1>🌱 Smart Plant Care</h1>

        <p class="subtitle">
            ESP8266 Akilli Bitki Bakim Sistemi
        </p>

        <div class="status" id="status">
            Baglanti kontrol ediliyor...
        </div>

        <div class="grid">

            <div class="card">
                <h3>🌡️ Sicaklik</h3>
                <div class="value temperature"
                     id="temperature">--</div>
            </div>

            <div class="card">
                <h3>💧 Hava Nemi</h3>
                <div class="value humidity"
                     id="humidity">--</div>
            </div>

            <div class="card">
                <h3>🌿 Toprak Nem Degeri</h3>
                <div class="value soil"
                     id="soil">--</div>
            </div>

        </div>

        <p class="footer">
            Veriler her 2 saniyede otomatik yenilenir.
            Toprak degeri kalibrasyon oncesi ham ADC verisidir.
        </p>

    </div>

    <script>
        async function verileriGuncelle() {
            try {
                const response = await fetch(
                    "/api/sensors",
                    { cache: "no-store" }
                );

                if (!response.ok) {
                    throw new Error("HTTP hatasi");
                }

                const data = await response.json();

                document.getElementById(
                    "temperature"
                ).innerText =
                    data.temperature === null
                    ? "--"
                    : data.temperature.toFixed(1) + " °C";

                document.getElementById(
                    "humidity"
                ).innerText =
                    data.humidity === null
                    ? "--"
                    : "%" + data.humidity.toFixed(1);

                document.getElementById(
                    "soil"
                ).innerText = data.soil;

                document.getElementById(
                    "status"
                ).innerText = "● ESP8266 Bagli";

            } catch (error) {
                document.getElementById(
                    "status"
                ).innerText = "● Baglanti hatasi";
            }
        }

        verileriGuncelle();

        setInterval(verileriGuncelle, 2000);
    </script>
</body>
</html>
)rawliteral";

void sensorleriOku() {
    toprakNem = analogRead(SOIL_PIN);
    sicaklik = dht.readTemperature();
    havaNem = dht.readHumidity();

    Serial.println("-----------------------");

    Serial.print("Toprak: ");
    Serial.println(toprakNem);

    Serial.print("Sicaklik: ");
    Serial.println(sicaklik);

    Serial.print("Hava nemi: ");
    Serial.println(havaNem);
}

void setup() {
    Serial.begin(115200);
    dht.begin();

    WiFi.mode(WIFI_STA);
    WiFi.begin("WIFI_ADI", "WIFI_SIFRESI");

    Serial.print("WiFi baglantisi kuruluyor");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("ESP8266 IP: ");
    Serial.println(WiFi.localIP());

    // Ana web sayfasi
    server.on("/", HTTP_GET, []() {
        server.send_P(
            200,
            "text/html; charset=utf-8",
            WEB_PAGE
        );
    });

    // Sensor verilerini JSON olarak gonder
    server.on("/api/sensors", HTTP_GET, []() {
        String json = "{";

        json += "\"temperature\":";
        json += isnan(sicaklik)
            ? "null" : String(sicaklik, 1);

        json += ",\"humidity\":";
        json += isnan(havaNem)
            ? "null" : String(havaNem, 1);

        json += ",\"soil\":";
        json += String(toprakNem);

        json += "}";

        server.sendHeader(
            "Cache-Control", "no-store"
        );

        server.send(
            200,
            "application/json",
            json
        );
    });

    server.begin();

    // Ilk sensor olcumu
    sensorleriOku();
    sonOlcum = millis();

    Serial.println("Web sunucusu baslatildi.");
}

void loop() {
    server.handleClient();

    if (millis() - sonOlcum >= olcumAraligi) {
        sonOlcum = millis();
        sensorleriOku();
    }
}

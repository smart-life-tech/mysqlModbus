#include <ModbusMaster.h>

#define MAX485_DE 18
#define MAX485_RE_NEG 19

#define MODBUS_DIR_PIN 17 // connect DR, RE pin of MAX485 to gpio 4
#define MODBUS_RX_PIN 18 // Rx pin
#define MODBUS_TX_PIN 19 // Tx pin
ModbusMaster node;

#define MODBUS_SERIAL_BAUD 9600 
struct RegisterInfo
{
    const char *label;
    const char *type;
    const char *dataRange;
    const char *accuracy;
    const char *dataType;
};

RegisterInfo registerInfo[] = {
    {"Reserved", "", "", "", ""},
    {"Reserved", "", "", "", ""},
    {"Mode", "R/W", "1:Heat/2:Hot water/3:Cool+Heat water/4:Heat+Hot water/5:Cool", "", "Unsigned Integer"},
    {"Optional E-Heater", "R/W", "1:1 set/2:2 sets/3: Off", "", "Unsigned Integer"},
    {"Disinfection Temp", "R/W", "40~70°C", "", "Unsigned Integer"},
    {"Floor Debug Segments", "R/W", "1-10 sections", "", "Unsigned Integer"},
    {"Floor Debug Period 1 temp", "R/W", "25~35°C", "", "Unsigned Integer"},
    {"ΔT of segment", "R/W", "2~10°C", "", "Unsigned Integer"},
    {"Segment time", "R/W", "12~72 Hours", "", "Unsigned Integer"},
    {"WOT-Cool", "R/W", "7~25°C", "", "Unsigned Integer"},
    {"WOT-Heat", "R/W", "20~60°C [High-temp]/20~55°C[low-temp]", "", "Unsigned Integer"},
    {"RT-Cool", "R/W", "18~30°C", "", "Unsigned Integer"},
    {"RT-Heat", "R/W", "18~30°C", "", "Unsigned Integer"},
    {"T-water tank", "R/W", "40~80°C", "", "Unsigned Integer"},
    {"T-Eheater", "R/W", "-20~18°C", "", "Unsigned Integer"},
    {"T-Other switch on", "R/W", "-20~18°C", "", "Unsigned Integer"},
    {"T-HP max", "R/W", "40~55°C", "", "Unsigned Integer"},
    {"Upper AT-Heat", "R/W", "10~37°C", "", "Unsigned Integer"},
    {"Lower AT-Heat", "R/W", "-20~9°C", "", "Unsigned Integer"},
    {"Upper RT-Heat", "R/W", "22~30°C", "", "Unsigned Integer"},
    {"Lower RT-Heat", "R/W", "18~21°C", "", "Unsigned Integer"},
    {"Upper WT-Heat", "R/W", "46~60°C[High-temp]/46~55°C[Low-temp]", "", "Unsigned Integer"},
    {"Lower WT-Heat", "R/W", "20~45°C", "", "Unsigned Integer"},
    {"Upper AT-Cool", "R/W", "26~48°C", "", "Unsigned Integer"},
    {"Lower AT-Cool", "R/W", "10~25°C", "", "Unsigned Integer"},
    {"Upper RT-Cool", "R/W", "24~30°C", "", "Unsigned Integer"},
    {"Lower RT-Cool", "R/W", "18~23°C", "", "Unsigned Integer"},
    {"Upper WT-Cool", "R/W", "15~25°C[with FCU]", "", "Unsigned Integer"},
    {"Lower WT-Cool", "R/W", "7~14°C", "", "Unsigned Integer"},
    {"ΔT-Cool", "R/W", "2~10°C", "", "Unsigned Integer"},
    {"ΔT-Heat", "R/W", "2~10°C", "", "Unsigned Integer"},
    {"ΔT-hot water", "R/W", "2~8°C", "", "Unsigned Integer"},
    {"ΔT-Room temp", "R/W", "1~5°C", "", "Unsigned Integer"},
    {"Cool run time", "R/W", "1~10 min", "", "Unsigned Integer"},
    {"Heat run time", "R/W", "1~10 min", "", "Unsigned Integer"},
    {"Other thermal logic", "R/W", "1~3", "", "Unsigned Integer"},
    {"Tank heater", "R/W", "1~2", "", "Unsigned Integer"},
    {"Optional E-Heater logic", "R/W", "1~2", "", "Unsigned Integer"},
    {"Current limit value", "R/W", "0~50A", "", "Unsigned Integer"},
    {"Thermostat", "R/W", "0: Without/1: Air/2: Air+hot water", "", "Unsigned Integer"},
    {"Force mode", "R/W", "1: Force-cool/2: Force-heat/3: Off", "", "Unsigned Integer"},
    {"Air removal", "R/W", "1: Air/2: Water tank/3: Off", "", "Unsigned Integer"},
    {"On/Off", "R/W", "0xAA: On/0x55: Off", "", "Unsigned Integer"},
    {"Power limit", "R/W", "0~10 Kw", "", "Unsigned Integer"},
    {"Error reset", "R/W", "0: Does not clear fault/1: Clear fault", "", "Unsigned Integer"}};

void modbusPreTransmission()
{
    delay(500);
    digitalWrite(MODBUS_DIR_PIN, HIGH);
}
// Pin 4 made low for Modbus receive mode
void modbusPostTransmission()
{
    digitalWrite(MODBUS_DIR_PIN, LOW);
    delay(500);
}

void preTransmission()
{
    digitalWrite(MAX485_RE_NEG, 1);
    digitalWrite(MAX485_DE, 1);
}

void postTransmission()
{
    digitalWrite(MAX485_RE_NEG, 0);
    digitalWrite(MAX485_DE, 0);
}

void setup()
{
    pinMode(MAX485_RE_NEG, OUTPUT);
    pinMode(MAX485_DE, OUTPUT);
    digitalWrite(MAX485_RE_NEG, 0);
    digitalWrite(MAX485_DE, 0);

    Serial.begin(115200);

    pinMode(MODBUS_DIR_PIN, OUTPUT);
    digitalWrite(MODBUS_DIR_PIN, LOW);

    // Serial2.begin(baud-rate, protocol, RX pin, TX pin);.
    Serial2.begin(MODBUS_SERIAL_BAUD, SERIAL_8E1, MODBUS_RX_PIN, MODBUS_TX_PIN);
    Serial2.setTimeout(200);
    // modbus slave ID 14
    node.begin(14, Serial2);
    node.begin(1, Serial);
    node.preTransmission(preTransmission);
    node.postTransmission(postTransmission);
}

void loop()
{
    uint16_t data[45]; // Array to store the read data

    // Read words 0 to 44 (registers 0 to 43) from Modbus slave with ID 1
    uint8_t result = node.readHoldingRegisters(0, 45);

    if (result == node.ku8MBSuccess)
    {
        // Reading successful, print the data
        for (int i = 0; i < 45; i++)
        {
            Serial.print("Word ");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(node.getResponseBuffer(i));
            Serial.print(" - ");
            Serial.print(registerInfo[i].label);
            Serial.print(" (");
            Serial.print(registerInfo[i].type);
            Serial.print(", ");
            Serial.print(registerInfo[i].dataRange);
            Serial.print(", ");
            Serial.print(registerInfo[i].accuracy);
            Serial.print(", ");
            Serial.print(registerInfo[i].dataType);
            Serial.println(")");
        }
    }
    else
    {
        // Reading failed, print error code
        Serial.print("Error reading data. Error code: ");
        Serial.println(result);
    }

    delay(1000); // Wait for 1 second before reading again
}

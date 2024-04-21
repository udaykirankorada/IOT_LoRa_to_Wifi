#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;

#define node_id_length 3 // Length of Node ID
#define sensor_1_pin A0  // Pin for sensor 1
#define sensor_2_pin A1  // Pin for sensor 2

char node_id[node_id_length] = {1, 1, 1}; // LoRa End Node ID
float frequency = 868.0;
unsigned int count = 1;
bool acknowledgmentReceived = false; // Flag to track acknowledgment status
byte prevcombinedbits = 0b11;

void setup()
{
    Serial.begin(9600);
    if (!rf95.init())
        Serial.println("init failed");
    // Setup ISM frequency
    rf95.setFrequency(frequency);
    // Setup Power,dBm
    rf95.setTxPower(13);
    rf95.setSyncWord(0x34);

    Serial.println("LoRa End Node Example --");
    Serial.println("    Sensor Data Sending\n");
    Serial.print("LoRa End Node ID: ");

    for (int i = 0; i < node_id_length; i++)
    {
        Serial.print(node_id[i], HEX);
    }
    Serial.println();
}

// Function to read sensor value and determine bit based on threshold
byte readSensorAndAssignBit(int sensorPin)
{
    int sensorValue = analogRead(sensorPin);
    return sensorValue > 125 ? 1 : 0;
}

int solve(byte bits, bool &ack, byte prev)
{
    if (bits == 0b11)
    {
        ack = false;
        return 0;
    }
    else if (bits == 0b01)
    {
        if (ack == false)
        {
            return 50;
        }
        else
        {
            return 20;
        }
    }
    return 0;
}

void loop()
{
    Serial.print("###########    ");
    Serial.print("COUNT=");
    Serial.print(count);
    Serial.println("    ###########");
    count++;

    // Read sensor values and assign bits
    byte bit1 = readSensorAndAssignBit(sensor_1_pin);
    byte bit2 = readSensorAndAssignBit(sensor_2_pin);

    // Combine the bits into one byte
    byte combinedBits = (bit1 << 1) | bit2;

    char data[node_id_length + 1] = {0}; // Payload Length (Node ID + Sensor Data)
    // Use node_id as the first bytes of data
    for (int i = 0; i < node_id_length; i++)
    {
        data[i] = node_id[i];
    }

    // Decide data to be sent based on acknowledgment status and combined bits
    int dataToSend = 0;
    dataToSend = solve(combinedBits, acknowledgmentReceived, prevcombinedbits);
    prevcombinedbits = combinedBits;

    // Print sensor values and combined bits
    Serial.print("Sensor 1 value: ");
    Serial.println(analogRead(sensor_1_pin));
    Serial.print("Sensor 2 value: ");
    Serial.println(analogRead(sensor_2_pin));
    Serial.print("Combined Bits: ");
    Serial.println(combinedBits, BIN);

    // Print data to be sent
    Serial.print("Data to be sent: ");
    Serial.println(dataToSend);

    // Set the last byte of data as the data to be sent
    data[node_id_length] = dataToSend;

    rf95.send((uint8_t *)data, sizeof(data)); // Send LoRa Data

    // Wait for acknowledgment

    if (rf95.waitAvailableTimeout(3000))
    {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; // receive data buffer
        uint8_t len = sizeof(buf);            // data buffer length
        if (rf95.recv(buf, &len))             // check if reply message is correct
        {
            if (buf[0] == node_id[0] && buf[1] == node_id[2] && buf[2] == node_id[2]) // Check if reply message has the our node ID
            {

                Serial.print("Got Reply from Gateway: "); // print reply
                Serial.println((char *)buf);
                acknowledgmentReceived = true;
            }
        }
        else
        {
            Serial.println("recv failed"); //
        }
    }
    else
    {
        Serial.println("No reply, is LoRa gateway running?"); // No signal reply
    }
    delay(3000); // Send sensor data every 3 seconds
    Serial.println("");
}

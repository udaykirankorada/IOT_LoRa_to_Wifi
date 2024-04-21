#include <SPI.h>
#include <RH_RF95.h>
#include <Console.h>
#include <Process.h>
RH_RF95 rf95;

// If you use Dragino IoT Mesh Firmware, uncomment below lines.
// For product: LG01.
#define BAUDRATE 115200

String myWriteAPIString = "P07KVY59P5QEY6M6";
float frequency = 868.0;

void setup()
{
    Bridge.begin(BAUDRATE);
    Console.begin();
    if (!rf95.init())
        Console.println("init failed");
    ;
    // Setup ISM frequency
    rf95.setFrequency(frequency);
    // Setup Power,dBm
    rf95.setTxPower(13);
    rf95.setSyncWord(0x34);

    Console.println("LoRa Gateway Example  --");
    Console.println("    Upload Single Data to ThinkSpeak");
}

void loop()
{
    if (rf95.waitAvailableTimeout(2000)) // Listen Data from LoRa Node
    {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; // receive data buffer
        uint8_t len = sizeof(buf);            // data buffer length
        if (rf95.recv(buf, &len))             // Check if there is incoming data
        {
            Console.print("Get LoRa Packet: ");
            for (int i = 0; i < len; i++)
            {
                Console.print(buf[i], HEX);
                Console.print(" ");
            }
            Console.println();

            if (len >= 4 && buf[0] == 1 && buf[1] == 1 && buf[2] == 1) // Check if the ID matches the LoRa Node ID
            {
                uint8_t randomValue = buf[3]; // Extract random value from received data
                uploadData(randomValue);      // Forward random value to ThingSpeak
            }
        }
    }
}

void uploadData(int randomValue)
{ // Upload Data to ThingSpeak
    // form the string for the API header parameter:

    // form the string for the URL parameter, be careful about the required "
    String upload_url = "https://api.thingspeak.com/update?api_key=";
    upload_url += myWriteAPIString;
    upload_url += "&field1=";
    upload_url += randomValue;

    Console.println("Call Linux Command to Send Data");
    Process p; // Create a process and call it "p", this process will execute a Linux curl command
    p.begin("curl");
    p.addParameter("-k");
    p.addParameter(upload_url);
    p.run(); // Run the process and wait for its termination

    Console.print("Feedback from Linux: ");
    // If there's output from Linux,
    // send it out the Console:
    while (p.available() > 0)
    {
        char c = p.read();
        Console.write(c);
    }
    Console.println("");
    Console.println("Call Finished");
    Console.println("####################################");
    Console.println("");
}

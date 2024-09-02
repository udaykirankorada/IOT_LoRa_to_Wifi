# Design_Lab

Topic: Conservation of energy in microgrid using IoT and smart switches

Requirements : 

a) Transmitter :
1) Designed PCB
2) Arduino + LoRa Shield 

b) Propagator :
1) LoRa Shield
2) Lora Gateway

c) Reciever : SmartLife Smart switch

Implemented Automation of turning off of device when inverter power detected. Modifiable to send notification as well to users. 

Once set up at generator, we detect whether power is common line power or generator power and using LoRa and wifi we transmit this information to a wifi switch which turns on and off as per command. The automation is achieved through thingspeak and IFTTT. Where encrypted http requests are sent through putty using Linux commands.

Demo video of automation added to the repo.

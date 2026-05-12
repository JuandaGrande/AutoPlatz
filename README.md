# AutoPlatz
### By: Cubillos, J., Garavito, J., Guerrero, M.
#### Justification
 Consists on an automated parking system in which the tolls are managed by a RC522 RFID Card Reader. It also features car detection on every place, leading to LED indication of avaliability and empty place counting displayed on the entrance.
#### Libraries
##### MRFC522 by miguelbalboa
This library assist completely the use of the RC522 rfid module, it integrates the reading and writing features of the module. We used it to read RFID chips in order to unlock the parking toll.
##### madhephaestus/ESP32Servo
This library Aids on the servo control, providing the PWM management in order to control the servos via degrees of movement
##### witnessmenow/UniversalTelegramBot @ ^1.3.0
This library provides the ESP with the capabilities to communicate with Telegram servers to let it be controlled via commands on the bot
##### bblanchon/ArduinoJson @ ^6.21.0
Dependencies from UniversalTelegramBot
#### Diagram    
 ![AutoPlatz Diagram](/SAP_Diagram.jpg)
#### Schematic
![AutoPlatz Schematic](/SAP_schematic.png)
Disclaimer: The proximity sensor used on the schematic may differe from the one used in the project. This due to limitations of schematic capabilities.


### Usage
#### Telegram Bot
In order to control the tolls via Telegram, you have to follow this steps:

##### 1. Find the Correct Bot
You can search for @juandagrande_AutoPlatzBot, or alternatively you can click ![here](https://t.me/juandagrande_AutoPlatzBot)
##### 2. Initialize Bot
Click on Start to see options and start the bot
##### 3. Select
###### 3.1 Open
Write /abrir and put the correct password to open the entrance toll
The bot will either give a confirmation message which tells the toll was opened or give an error nessage if the password is not correct
###### 3.2 Status
Write /estado
The bot will display which places are occupied.

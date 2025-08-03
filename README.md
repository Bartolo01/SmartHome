Firstly, this is not a "plug and play" project. If u want to use it, u will need to be able to program and have a basic knowledge how the code function.
The code is a rough, but works well and is reliable.

The Master code is made for any esp32 board, but I am using "ESP32 DOIT DEVKIT V1". It hosts website with interactive UI and sends updates to the slave boards which "do stuff". The communication happens through ESP-NOW.

There is no possibility of adding new devices on the website, so u will need to hard code them into a HTTML part of the code. There are examples there for a ON-OFF switch, SLIDER for pwm or anything really and a simple sensor reading.
To add any new devices there is need also for adding a new ONSERVER command. U should be able to do it with chatGPT.
  
The Slave code is made for additional devices. There is a template mode for my custom boards with esp32c3 module. There is also a "Bartek" code that is running in my home. I put it there as an example for other people.

I am also adding a .rar which contains files for the custom slave boards. U can send them directly to manufacterer.

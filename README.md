# RC-CORNER-SCALE
Arduino based RC Corner Weight Scale. DIY/Cheap Solution
Hello All,
Lets make a DIY RC-CORNER-WEIGHT SCALE

Items Needed

1. Load Cell 2 Kg -  4
2. HX711 ADC Module -  4
3. Arduino Uno -1
4. LCD 16X2 i2c – 1
5. Cables- as required
6. Soldering Iron
7. Push Buttons - 2
8. Plastic box -1

Steps involved.

1. Wire the Loadcell, ADC and Arduino as in the picture. Fritz file is attacthed.

2. Download the LCD and HX711 ADC library and include in the IDE project.

HX711_ADC Library https://github.com/olkal/HX711_ADC
LCD i2C Library https://www.arduinolibraries.info/libraries/liquid-crystal-i2-c


3. Calibrating the Loadcell's

Before using Loadcells for measuring each of them have to be calibrated.

For that open the sketch "Calibration" from Examples of HX711_ADC library.

Edit the calibration file Dout as A0 and SCK as 2. Uploading this sketch will calibrate the Front Right Loadcell Only.

Download the calibraion sketch to Arduino and open the Serial Plotter. 
[Select correct baudrate, else messages wont be displayed correctly]. 

Ensure no loads are placed on loadcell, enter "t" and send. Tare operation is performed. 

Place a known weight on the platform. For example I have placed 60.0 grams enter 60.0 and send.

Calibration factor for the this loadcell will be displayed.

Note the Calibration factor. 

Edit the calibration file Dout as A1 and SCK as 3. Uploading this sketch will calibrate the Front Left Loadcell Only.

Note the Calibration factor. 

Repeat the steps for other two loadcells.

4. Copy paste the code.

Note: Edit the calibration factor obtained for each laodcell in main sketch.

  calibrationValue_1 = 1070.13; 
	
  calibrationValue_2 = 1145.97; 
	
  calibrationValue_3 = 1154.0; 
	
  calibrationValue_4 = 1056.95; 
	
In my case above values are obtained. You must Enter your loadcell's value else it wont work correctly.

5. Upload.



# Mini-Fridge Fermenter Code

Arduio Nano 33 IOT code to power a mini-fridge for sausage and cheese fermentation, beer fermentaions, dry-aging meat, koji and tempeh, etc.

## My Setup

The display:

<img src="https://github.com/verysmallrock/FridgeFermenter/blob/main/readmedata/display.jpg?raw=true" width="300"/>

The thing aging food:

<img src="https://github.com/verysmallrock/FridgeFermenter/blob/main/readmedata/fridge.jpg?raw=true" width="300"/>

## Parts List

These are the pieces I used. You could certainly use other parts! Knowing the names of the things I used (like "wire nuts") will hopefully be helpful. Having the right tools around is something I've accumulated for myself over years and I haven't listed all the tools I used, so keep that in mind.

(Yes, these are affiliate links - feel free to get around that if you'd like to).

- [Arduino Nano 33 IOT](https://store.arduino.cc/usa/nano-33-iot).
- [Temp/Humidity Sensor](https://www.amazon.com/gp/product/B0795F19W6?ie=UTF8&psc=1&linkCode=ll1&tag=lukrym-20&linkId=b486d25ab6e4b8790191439777602fc0&language=en_US&ref_=as_li_ss_tl).
- [4-Channel 5V Relay Module](https://www.amazon.com/gp/product/B00KTEN3TM?ie=UTF8&psc=1&linkCode=ll1&tag=lukrym-20&linkId=ff5f6f848a3d0b86bc1fc15f77313c02&language=en_US&ref_=as_li_ss_tl).
- [4.5x3.5x2.2 Project Box](https://www.amazon.com/gp/product/B083H9FNRT?ie=UTF8&psc=1&linkCode=ll1&tag=lukrym-20&linkId=cb9c5620ddc8028527556253d6c852c3&language=en_US&ref_=as_li_ss_tl).
- Mini Fridge (3.2 Cubic Foot style), freezer removed and cooling elements adjusted (i.e. carefully bent) to make space.
- Small Humidifier with always-on switch. I used a [Homasy OceanMist 2.2L Humidifier](https://www.amazon.com/gp/product/B07RZSBSHJ?ie=UTF8&psc=1&linkCode=ll1&tag=lukrym-20&linkId=87576ba18dd32a623d075564934d3fa7&language=en_US&ref_=as_li_ss_tl).
- Small De-Humidifier with always-on switch. I used a [Beyxdu 500ml Mini-Humidifer](https://www.amazon.com/gp/product/B08GZ5X3LS?ie=UTF8&psc=1&linkCode=ll1&tag=lukrym-20&linkId=87047495e5704d8dc332e4ea6d7e05ae&language=en_US&ref_=as_li_ss_tl).
- Small Heating element (optional, depending on application). I used a [100W Ceramic Heat Lamp](https://www.amazon.com/gp/product/B078TDWPW5?ie=UTF8&psc=1&linkCode=ll1&tag=lukrym-20&linkId=cf68c355be76eacd109e89950d577606&language=en_US&ref_=as_li_ss_tl).
- [12V Fans](https://www.amazon.com/gp/product/B002YFSHPY?ie=UTF8&psc=1&linkCode=ll1&tag=lukrym-20&linkId=10a59ab0ad20ab55b6fab55db6f1edae&language=en_US&ref_=as_li_ss_tl).  Nothing fancy needed here since we're powering them at 5V and not using speed monitoring or speed-control (i.e. you only need the two-wire kind). I use three fans - one inside to circulate air (always on), and two to pull air in and push air out (periodic activation).
- The smallest USB phone charger you can find, which will be packed into your project box to power the arduino.
- The right tools for the job (wire cutters, wire strippers, wire crimpers, soldering iron, heat gun, dremel, screwdriver, pliers, etc).
- If you want, some kind of wire connectors. I cannibalized some PWM fan connectors leftover from a PC build.
- Wiring bits
    - [breadboard and dupont wires](https://www.amazon.com/gp/product/B073X7GZ1P?ie=UTF8&psc=1&linkCode=ll1&tag=lukrym-20&linkId=7bf6e074223fc1dd57e1be55bdc61fdc&language=en_US&ref_=as_li_ss_tl)
	- [120V extension cables](https://www.amazon.com/gp/product/B0153T1J8Y?ie=UTF8&psc=1&linkCode=ll1&tag=lukrym-20&linkId=be3e7c630bf3fb102ec836a4fbd1e33e&language=en_US&ref_=as_li_ss_tl)
	- [Wire nuts](https://www.amazon.com/Insulating-Electrical-Connectors-Portable-Connection/dp/B08DHZHQ4M?dchild=1&keywords=wire+nuts&qid=1619153897&sr=8-3&linkCode=ll1&tag=lukrym-20&linkId=99f28838b6a6ac1eec17d4aacd88e34d&language=en_US&ref_=as_li_ss_tl)

## Config

Create a `Secrets.h` file that looks like this:

```
#ifndef SSID
#define SSID "WIFISSD"
#define PASSWORD "WIFIPASSWORD"
#define SECRET_GSCRIPT_ID "GOOGLESHEET SCRIPT ID"
#endif

```

TBD Details on the Google sheet.

## Notes on Fonts

Feel free to use my fonts.  You can drop them in to the Adafruit_ST7735 library easily.

- [FreeUniversal Bold](https://github.com/verysmallrock/FridgeFermenter/blob/main/TitleFont.h) - my large font, used only as a single line at the top of the screen.
- [Helvetica B10](https://github.com/verysmallrock/FridgeFermenter/blob/main/HelvB10.h) - My main UI font. 7 lines per screen not counting the title at the top.
- [Tiny Font](https://github.com/verysmallrock/FridgeFermenter/blob/main/TinyFont.h) - A tiny font I use for overlaying on graphs.

### My Process & Requirements

The fonts in Adafruit_GFX are not very good for the sizes I needed, so I created my own.  My requirements:

- [128×160px LCD](https://www.ebay.com/itm/224237573577) - ST7735S 128x160 QVGA display
- One large font for title, 4 lines per screen or so (only used as a title though)
- One medium font for UI, 7 lines per screen
- One tiny font for drawing on graphs

This is what I'm talking about:

<img src="https://github.com/verysmallrock/FridgeFermenter/blob/main/readmedata/display2.jpg?raw=true" width="300"/>

This is the general procedure I took for fonts:

1. Source a font (BDF format was easiest to work with, but TTF can be used also).
2. Convert the font into a format usable by Arduino (using binFontsTool).
3. Try out the font and write some code to adjust it as needed.

### Technical details on creating my fonts

1. Get [UCGLib](https://github.com/olikraus/ucglib) and build the tools in `tools/font`
2. Get [binFontsTool](https://forum.arduino.cc/t/excel-fonts-editor-converter-for-adafruit-utft-squix-ili9341_t3-oled_i2c/430653) (an Excel spreadsheet) - The version I used is [here](https://github.com/verysmallrock/FridgeFermenter/blob/main/readmedata/binFontsTool-0.2.7.zip?raw=true).
2. *If font is TTF*, convert to BDF using ucglib: `do_fontsize_a_v2.sh 22 {source_ttf} {destination}`
3. Run the BDF for a font you like through binFontsTool to create the .h file (the ones I used are in [readmedata](https://github.com/verysmallrock/FridgeFermenter/tree/main/readmedata)).
4. Try out the font in you project, and adjust.  I wrote a [function to adjust vertical position](https://github.com/verysmallrock/FridgeFermenter/blob/main/Display.cpp#L45).
5. Use the font!

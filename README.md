# CaseLights
PC case lighting with Arduino and NeoPixels


## Serial Interface
Commands are accepted interactively via the serial interface.

Comms parameters: 9600,8,1,N

Note that PuTTY should be configured to generate Control-H (0x08) for the backspace key.


## Supported commands
* __C[rrggbb][ nn]__ - Static colour [RGB value in hex, default random] [pixel index, default all]
* __K[rrggbb][ nn]__ - Komet, colour brightness decreases along strip [RGB value of comet head in hex, default random] [length in pixels, default all]
* __J 00__ - Jump the current pattern n steps along (wraps around end of buffer)
* __X__ - Set all pixels to a different random colour
* __B[rrggbb][ nnnn]__ - Blink colour [RGB value in hex, default current][interval in ms, default random]
* __F[rrggbb][ nnnn]__ - Fade colour up and down [RGB value in hex, default current][interval in ms, default random]
* __R[ nnnn]__ - Rotate clockwise [interval in ms, default random]
* __W[ nnnn]__ - Rotate widdershins [interval in ms, default random]
* __S[ nnnn][ nn]__ - Shuttle (back and forth) [interval in decimal ms, default random] [number of pixels decimal, default all]
* __N[ nnnn][ nn]__ - Bounce (back and forth, reversing direction) [interval in decimal ms, default random] [number of pixels decimal, default all]
* __H__ - Halt (animation)
* __P__ - Persist current config in EEPROM (survives restart)
* __A__ - Activate light zone
* __D__ - Deactivate light zone

## Notes on Command Syntax
Commands are accepted in upper or lowercase.

Square brackets [] indicate an optional parameter.

Note that whitespace, where specified above, is NOT optional, and must be exactly adhered to

__rrggbb__ - an RGB colour value in hex (upper and lower case accepted), must be exactly 6 digits long, always follows the command character WITHOUT an intervening space

__nnnnn__ - a decimal value, generally a time parameter in ms, must be exactly 4 digits long (use leading zeroes), always preceded by a single space character

__nn__ - a decimal value, generally a pixel index or count, must be exactly 2 digits long (use leading zeroes), always preceded by a single space character


## Multiple Zones
The system supports 3 independently-programmable zones. To specify a zone, precede the command with the single-digit zone ID (0..2).

Note that no space is allowed between the zone ID and the command character.

If no zone is specified, zone 0 is assumed by default. The zones can be activated and deactivated by means of the A and D commands.

The number of zones is limited by the available RAM in the controller.


## Persistence
By using the P command, the current state of all light zones can be stored persistently to EEPROM, in order to resume the programmed operation after a system reset.

If no valid persistent data is found upon system start, the LED strip defaults to a uniform dimmish green.


## Return values
* __OK__ - command was executed
* __E_CMD__ - Unknown command, bad paramter, etc.
* __E_LEN__ - Command buffer overflow (max 20 chars)
* __E_IDX__ - Invalid light zone index


## Helper Script
Because it can be frustrating and error-prone to enter commands interactively, a helper-script (Python) is provided: LightFandango.py

By means of this script, a text file containing multiple commands can be built up and sent to the controller in an automated fashion.
```
python LightFandango.py COM3 Test.txt
```

Please see the script's help for further options:
```
python LightFandango.py --help
```

NOTE: Connecting and disconnecting to the Arduino via the serial port causes the controller to reset. For this reason, LightFandango performs a short countdown before attempting to send any commands. This defaults to 3 seconds, but can be configured via command-line parameter. Also for this reason, the final command of the file being sent should be P, otherwise the changes made will immediately be discarded when the controller restarts after closing the serial connection.

Blank lines are allowed in the script, and will be ignored.

Comment lines in the script must be preceded by a #

The special command **!delay** (or **!DELAY**) forces a pause in the transmission of data to the controller (specified in seconds), e.g.:
```
    C000000
    !delay 2
    C660000
    !delay 2
    C000066
    !delay 2
    C006600
    !delay 2
```

The delay feature is specifically useful in conjunction with the __--repeat__ command-line option.


## TODO
Bounce mode

Can a fourth zone be made to fit?
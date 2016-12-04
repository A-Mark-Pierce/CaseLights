# CaseLights
PC case lighting with Arduino and NeoPixels

Akzeptiert Kommandos per serielle Schnitstelle.

Unterstuezte Kommandos:
C[000000][ 00] - Fixed colour [RGB value in hex, default random] [pixel index decimal, default all]
K[000000] - Komet, colour brightness decreases along strip [RGB value of comet head in hex, default random]
X - Set all pixels to a different random colour
B[000000][ 0000] - Blink colour [RGB value in hex, default current][interval in decimal ms, default random]
F[000000][ 0000] - Fade colour up and down [RGB value in hex, default current][interval in decimal ms, default random]
R[ 0000] - Rotate clockwise [interval in decimal ms, default random]
W[ 0000] - Rotate widdershins [interval in decimal ms, default random]
S[ 0000][ 00] - Shuttle (back and forth) [interval in decimal ms, default random] [number of pixels decimal, default all]
H - Halt (animation)

ACHTE auf Grossschreibung, Whitespace usw.

Rueckgabe:
OK - Wird gemacht
E_CMD - Unbekanntes Kommando, falsche Parametrierung, usw.
E_LEN - Kommando-Puffer voll (kann max 20 Zeichen)
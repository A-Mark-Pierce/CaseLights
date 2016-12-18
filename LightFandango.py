#!/usr/bin/python

#
# Reads the contents of a text file and transmits them line-by-line to an Arduino 
# running the CaseLight LED controller code
#
# Example usage:
# python LightFandango.py COM3 TestValidCommands.txt -v
#
# Author: Mark Pierce
# December 2016
#

import sys
import serial
import time
import re
import argparse

parser = argparse.ArgumentParser(description='Send text commands from file to CaseLight controller.')

parser.add_argument("port", help="COM port to which the CaseLight controller is connected")
parser.add_argument("source", help="File containing the commands to be sent to the CaseLight controller")
parser.add_argument("-v", "--verbose", help="Verbose mode, displays all commands sent", action="store_true")
parser.add_argument("-b", "--baudrate", type=int, default=9600, help="Baud rate for serial comms")
parser.add_argument("-d", "--delay", type=int, default=3, help="Delay in seconds after opening serial port, to allow Arduino to reset")
parser.add_argument("-r", "--repeat", type=int, default=1, help="Number of times to repeat sending the file, 0 == infinite")
args = parser.parse_args()


serArduino = serial.Serial(args.port, args.baudrate, timeout=1, xonxoff=True)

# Warte, bis Arduino Reset durch ist
print("Waiting for Arduino reset...")
countDown = args.delay
while (countDown > 0):
	print(countDown)
	time.sleep(1)
	countDown = countDown - 1

numRepeat = args.repeat

while ( (0 == args.repeat) or (numRepeat > 0) ):
	lineCounter = 0
	with open(args.source, "r") as fileInput:
		for lightCmd in fileInput:
			lineCounter = lineCounter + 1
			lightCmd = lightCmd.strip()
			
			if ( (lightCmd == "") or (lightCmd[0] == "#") ):
				continue
				
			if (args.verbose):
				print( lightCmd )
				
			if (lightCmd[0] == "!"):
				if ("!DELAY" == lightCmd[0:6].upper()):
					nDelay = int(re.search(r'\d+', lightCmd).group())
					time.sleep(nDelay)
			else:
				lightCmd += "\r"	
				serArduino.write(lightCmd.encode("ascii"))
				response=""
				while ( True ):
					respChar = serArduino.read(1)
					if (respChar <= b"\x80" ):
					
						if (respChar == b"" ):
							break
							
						if (respChar == b"\r"):
							if ( response[0:2] == "E_" ):
								print( "Error: " + response + " at line " + str(lineCounter) + " : " + lightCmd )
							response = ""
						elif (respChar != b"\n"):
							response += respChar.decode("ascii")

					
	numRepeat = numRepeat - 1
	
time.sleep(1)
serArduino.close();
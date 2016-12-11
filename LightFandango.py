#!/usr/bin/python

#
# Reads the contents of a text file and transmits them line-by-line to an Arduino 
# running the CaseLight LED controller code
#
# Author: Mark Pierce
# December 2016
#

import sys
import serial
import time

import argparse

parser = argparse.ArgumentParser(description='Send text commands from file to CaseLight controller.')

parser.add_argument("source", help="File containing the commands to be sent to the CaseLight controller")
parser.add_argument("port", help="COM port to which the CaseLight controller is connected")
parser.add_argument("-v", "--verbose", help="Verbose mode, displays all commands sent", action="store_true")
parser.add_argument("-b", "--baudrate", type=int, default=9600, help="Baud rate for serial comms")
parser.add_argument("-d", "--delay", type=int, default=3, help="Delay in seconds after opening serial port, to allow Arduino to reset")
args = parser.parse_args()


serArduino = serial.Serial(args.port, args.baudrate, timeout=1, xonxoff=True)

# Warte, bis Arduino Reset durch ist
print("Waiting for Arduino reset...")
countDown = args.delay
while (countDown > 0):
	print(countDown)
	time.sleep(1)
	countDown = countDown - 1

lineCounter = 0
with open(args.source, "r") as fileInput:
	for lightCmd in fileInput:
		lineCounter = lineCounter + 1
		lightCmd = lightCmd.strip()
		
		if ( (lightCmd == "") or (lightCmd[0] == "#") ):
			continue
			
		if (args.verbose):
			print( lightCmd )
			
		lightCmd += "\r"	
		serArduino.write(lightCmd.encode("ascii"))
		response=""
		while ( True ):
			respChar = serArduino.read(1)
			if (respChar <= b"\x80" ):
			
				if (respChar == b"" ):
					break
					
				if (respChar == b"\r"):
#					print( response )
					response = ""
				elif (respChar != b"\n"):
					response += respChar.decode("ascii")
#					print(response)

				if (response == "OK"):
					break

				if ((response == "E_CMD") or (response == "E_LEN")):
					print( "Error at line " + str(lineCounter) + " : " + lightCmd )
					break
					
				
serArduino.close();
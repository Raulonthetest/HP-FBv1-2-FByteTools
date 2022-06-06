# HP-FBv1-2-FByteTools
Program used to convert HP FlexBuild 1.x/2.0 FByte information to DMI Type 11 and back
# Usage
HPFByteDMITool <type> <string>
where:
- type: conversion source: -t11 - DMI T11, -fbyte - FByte sticker
- string: string from DMI Type 11 (if /d is used) or FByte sticker (if /f is used). Examples: bid=YSRGOBXXXR;ABC;DEF... (DMI); 2S2U 3233 3E3K.../2S2U32333E3K... (BID and FByte sticker on product)
# Notes
This program uses the FByte information of HP's CPC_DMI tool's dmi.lib which needs to be decompressed before usage.
To decompress it, you need the "HPDmiLibDecompression" tool. 
This does not include the .lib to .ini tool required by the tool, which needs to be provided by the user 
Note: set an environment variable in CMD named "HPDMILIBTOOL" with the path of 7-zip with the set command
e.g.: set HPDMILIBTOOL="C:\Program Files\7-Zip"

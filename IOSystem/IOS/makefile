Target = MCPEMAPS.ipa


main: binFile
	node iosDevelopmentBridge.js uninstall com.mojang.minecraftpe
	node iosDevelopmentBridge.js install ${Target}

binFile: 
	if not exist "bin" mkdir bin
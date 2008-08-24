
installAutoCP(choose file with prompt "Choose AutoCP plugin for Hugin" of type {"BNDL", "????"} without invisibles)
--installAutoCP(pluginNextToMeCalled("Autopano-SIFT-C"))

to open the droppedFiles
	repeat with eachFile in droppedFiles
		installAutoCP(eachFile)
	end repeat
end open


to installAutoCP(theFile)
	tell application "Finder" to set theExtension to the name extension of theFile
	if theExtension is not equal to "huginAutoCP" then
		display alert "Wrong file type" buttons {"Quit"} as critical message ¬
			"The file extension ." & theExtension & " indicates this file is not an AutoCP plugin for Hugin."
		quit
	end if
	
	set theDestination to (the path to application support from user domain as text) & "Hugin:Autopano:"
	tell application "Finder"
		if not (exists theDestination) then do shell script ("mkdir -p " & quoted form of POSIX path of theDestination)
		if (exists (theDestination & the name of theFile)) then delete (theDestination & the name of theFile)
		set installedLocation to (duplicate theFile to theDestination) as text
	end tell
	
	display alert "Done" as informational message ¬
		"The AutoCP plugin has been successfully installed to:" & return & ¬
		return & ¬
		(swapDelimeters of installedLocation to " ▸ " instead of ":")
end installAutoCP

to swapDelimeters of theText to newDelim instead of oldDelim
	set defaultDelim to text item delimiters
	set text item delimiters to oldDelim
	set theItemsInTheText to theText's text items
	set text item delimiters to newDelim
	set theNewText to theItemsInTheText as string
	set text item delimiters to defaultDelim
	return theNewText
end swapDelimeters

on pluginNextToMeCalled(pluginName)
	tell application "Finder" to copy container of (path to me) as text to myContainer
	return alias (myContainer & pluginName & ".huginAutoCP")
end pluginNextToMeCalled
'script should be in the 'rFactor 2\Data\ModDev\Locations' folder
'________________________________________________________________

root = ".\"

JoesvilleFolder = root + "Joesville"
CheckFolderNotExists JoesvilleFolder

enteredTrackName = InputBox("Enter new track name (you can use spaces as they will be automatically replaced with underscores)")
CheckTrackName enteredTrackName

TrackName = Replace(enteredTrackName, " ", "_")
TrackFolder = root + TrackName
CheckFolderExists TrackFolder

CopyFolder JoesvilleFolder, TrackFolder

RenameFolder TrackFolder + "\JoesVille_Speedway", TrackFolder + "\" + TrackName
RenameFile TrackFolder + "\Joesville_Speedway.tdf", TrackFolder + "\" + TrackName + ".tdf"
RenameFile TrackFolder + "\" + TrackName + "\Joesville_Speedway.gdb", TrackFolder + "\" + TrackName + "\" + TrackName + ".gdb"
RenameFile TrackFolder + "\" + TrackName + "\Joesville_Speedway.scn", TrackFolder + "\" + TrackName + "\" + TrackName + ".scn"
RenameFile TrackFolder + "\" + TrackName + "\Joesville_Speedwayicon.tga", TrackFolder + "\" + TrackName + "\" + TrackName + "icon.tga"
RenameFile TrackFolder + "\" + TrackName + "\Joesville_SpeedwaySMicon.dds", TrackFolder + "\" + TrackName + "\" + TrackName + "SMicon.dds"
RenameFile TrackFolder + "\" + TrackName + "\Joesville_Speedway_loading.jpg", TrackFolder + "\" + TrackName + "\" + TrackName + "_loading.jpg"
RenameFile TrackFolder + "\" + TrackName + "\Joesville_SpeedwayThmb.tga", TrackFolder + "\" + TrackName + "\" + TrackName + "Thmb.tga"
RenameFile TrackFolder + "\JoesvilleIcon.dds", TrackFolder + "\" + TrackName + "Icon.dds"

DeleteFilesExceptExtensions TrackFolder + "\" + TrackName, "GDB;SCN;DDS;TGA;JPG"

DeleteFiles TrackFolder + "\Assets\GMT"
DeleteFiles TrackFolder + "\Assets\Maps"

gdbContentOld = ReadFile(TrackFolder + "\" + TrackName + "\" + TrackName + ".gdb")

gdbContentNew = ChangeGdbContent(gdbContentOld, TrackName, enteredTrackName)
WriteFile TrackFolder + "\" + TrackName + "\" + TrackName + ".gdb", gdbContentNew

scnContentOld = ReadFile(TrackFolder + "\" + TrackName + "\" + TrackName + ".scn")
scnContentNew = ChangeScnContent(scnContentOld, TrackName)
WriteFile TrackFolder + "\" + TrackName + "\" + TrackName + ".scn", scnContentNew

msgbox "DONE"



Function CheckTrackName(enteredTrackName)
 If enteredTrackName = "" Then
    msgbox  "You did not enter track name or you pressed cancel, aborting script!"
    Wscript.Quit
 End If
End Function

Function CheckFolderNotExists(source)
 set filesys = CreateObject("Scripting.FileSystemObject")
 If NOT filesys.FolderExists(source) Then
    msgbox "This script is NOT in the right folder. Copy it in the 'rFactor 2\Data\ModDev\Locations' folder, aborting script!"
    Wscript.Quit
 End If
End Function

Function CheckFolderExists(source)
 set filesys = CreateObject("Scripting.FileSystemObject")
 If filesys.FolderExists(source) Then
    msgbox "The entered folder '" + source + "' already exists, aborting script!"
    Wscript.Quit
 End If
End Function

Function CopyFolder(source, target)
 set filesys = CreateObject("Scripting.FileSystemObject")
 If filesys.FolderExists(source) Then
    filesys.CopyFolder source, target
 End If
End Function

Function RenameFile(source, target)
 set filesys = CreateObject("Scripting.FileSystemObject")
 If filesys.FileExists(source) Then
    filesys.MoveFile source, target
 End If 
End Function

Function RenameFolder(source, target)
 set filesys = CreateObject("Scripting.FileSystemObject")
 If filesys.FolderExists(source) Then
    filesys.MoveFolder source, target
 End If 
End Function

Function DeleteFilesExceptExtensions(Folder, Exceptions)
 Set objFSO = CreateObject("Scripting.FileSystemObject")
 
 If NOT objFSO.FolderExists(Folder) Then
 	Exit Function
 End If
 
 Set objFldr = objFSO.GetFolder(Folder)
 For Each objFile In objFldr.Files
    sTemp = Split(objFile.Name, ".")(1)
    If NOT InStrRev(LCase(Exceptions), LCase(sTemp)) > 0 Then
        objFSO.DeleteFile objFile.Path
    End If
 Next
End Function

Function DeleteFilesExceptFiles(Folder, Exceptions)
 Set objFSO = CreateObject("Scripting.FileSystemObject")
 Set objFldr = objFSO.GetFolder(Folder)
 For Each objFile In objFldr.Files
    sTemp = objFile.Name
    If NOT InStrRev(LCase(Exceptions), LCase(sTemp)) > 0 Then
        objFSO.DeleteFile objFile.Path
    End If
 Next
End Function

Function DeleteFile(target)
 set filesys = CreateObject("Scripting.FileSystemObject")
 If filesys.FileExists(target) Then
    filesys.DeleteFile target
 End If
End Function

Function DeleteFiles(Folder)
 Set objFSO = CreateObject("Scripting.FileSystemObject")
 Set objFldr = objFSO.GetFolder(Folder)
 For Each objFile In objFldr.Files
    objFSO.DeleteFile objFile.Path
 Next
End Function

Function ReadFile(sFile)
 Set objFSO = CreateObject("Scripting.FileSystemObject")
 
 If NOT objFSO.FileExists(sFile) Then
 	Exit function
 End If
 
 Set oTS = objFSO.OpenTextFile(sFile, 1)
 sText = oTS.ReadAll
 oTS.close
 ReadFile = sText
End Function

Function WriteFile(sFile, content)
 Set objFSO = CreateObject("Scripting.FileSystemObject")
 
 If NOT objFSO.FileExists(sFile) Then
 	Exit function
 End If
 
 set objFile = objFSO.OpenTextFile(sFile, 2)
 objFile.Write content
 objFile.Close
End Function

Function ChangeGdbContent(gdbContentOld, TrackName, enteredTrackName)
 newContent = gdbContentOld

 newContent = Replace(newContent, "Joesville_Speedway", enteredTrackName)
 newContent = Replace(newContent, "Joesville Speedway", enteredTrackName)
 newContent = Replace(newContent, "Joesville Runabout", enteredTrackName)
 newContent = Replace(newContent, "Joesville", enteredTrackName)
 ChangeGdbContent = newContent
 
End Function

Function ChangeScnContent(scnContentOld, TrackName)
 newContent = scnContentOld
  
 
 'nuevo
 coment="//-------------------------------------------------------" & vbCrLf
 p1=InStr(newContent,"LIGHTING")
 tmp=Left(newContent,p1-1) & "LIGHTING--------------------------" & vbCrLf & coment & vbCrLf & vbCrLf
 tmp =tmp & coment & "//----------------------TERRAIN--------------------------" & vbCrLf & coment & vbCrLf & vbCrLf
 tmp =tmp & coment & "//----------------------BARRIERS-------------------------" & vbCrLf & coment & vbCrLf & vbCrLf
 tmp =tmp & coment & "//----------------------TSO------------------------------" & vbCrLf & coment & vbCrLf & vbCrLf
 tmp =tmp & coment & "//----------------------VEGETATION-----------------------" & vbCrLf & coment & vbCrLf & vbCrLf
 tmp =tmp & coment & "//----------------------GRANDSTAND-----------------------" & vbCrLf & coment & vbCrLf & vbCrLf
 tmp =tmp & coment & "//----------------------VEHICLES-------------------------" & vbCrLf & coment & vbCrLf & vbCrLf
 tmp =tmp & coment & "//----------------------MOVABLE OBJECTS------------------" & vbCrLf & coment & vbCrLf & vbCrLf
 tmp =tmp & coment & "//----------------------ANIMATED OBJECTS-----------------" & vbCrLf & coment & vbCrLf & vbCrLf
 tmp =tmp & coment & "//----------------------MANDATORY OBJECTS----------------" & vbCrLf & coment & vbCrLf & vbCrLf
 tmp =tmp & coment & "//----------------------REFLECTION MAPS------------------" & vbCrLf & coment 

 tmp = Replace(tmp, "JOESVILLE", TrackName, 1, -1, 1)
 tmp = Replace(tmp, "_SPEEDWAY", "", 1, -1, 1)
 ChangescnContent = tmp
 Exit Function
 'fin nuevo

	numero= InStr(newContent, "Instance=lpole01")
	If (numero=0) Then
		Exit function
	End If
	
 newContent = Left(newContent, InStr(newContent, "Instance=lpole01") - 1)

 lightStart = InStr(newContent, "Light=")
 lightEnd = InStr( InStrRev(newContent, "Light="), newContent, "}")
 newContent = Replace(newContent, Mid(newContent, lightStart, lightEnd - lightStart + 1), "")

 newContent = Replace(newContent, "//MASFile=Joesville_Maps.mas" + vbCrLf, "")
 newContent = Replace(newContent, "//MASFile=Joesville_Anims.mas" + vbCrLf, "")
 newContent = Replace(newContent, "//MASFile=Joesville_SponsorMaps.mas" + vbCrLf, "")
 newContent = Replace(newContent, "//MASFile=Joesville_GMTs.mas" + vbCrLf, "")

 newContent = Replace(newContent, "SearchPath=JOESVILLE\ASSETS\MAPS", "SearchPath=JOESVILLE\ASSETS\MAPS" + vbCrLf + "SearchPath=JOESVILLE\ASSETS\JOESVILLE_MAPS")
 newContent = Replace(newContent, "SearchPath=JOESVILLE", "SearchPath=" + UCase(TrackName))

 ChangescnContent = newContent
End Function


'script by Abriel Nei. Modified by Spaskis for converting tracks from rf1 to rf2.
'modificado por Corti y compadre
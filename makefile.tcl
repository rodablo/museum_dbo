###
###
###
#package require registry 1.0 

### 
proc get_next_build_number {} {
    file attributes lastbuild -readonly 0
    set file [open lastbuild r+]
    gets $file number
    incr number
    seek $file 0
    puts $file $number
    close $file
    file attributes lastbuild -readonly 1
    puts "Build #$number"
    return $number
}

###
proc process_versioninfo {output_fname input_fname} {
    # lee el file
    set file [open $input_fname]
    set data [read $file]
    close $file
    # substituye
    regsub -all __VER_BUILD $data [get_next_build_number] newdata
    # crea el file de salida read only
    if [file exist $output_fname] { file delete $output_fname }
    set file [open $output_fname w 0444]
    puts $file "// Archivo generado NO EDITAR !!\n\n$newdata"
    close $file
}
###
proc process_dboinf {output_fname input_fname} {
    # lee el file
    set file [open $input_fname]
    set data [read $file]
    close $file
    # lee el last build
    set file [open lastbuild]
    gets $file build
    close $file
    # substituye
    regsub -all __VER_BUILD $data $build newdata
    # crea el file de salida read only
    if [file exist $output_fname] { file delete $output_fname }
    set file [open $output_fname w 0444]
    puts $file $newdata
    close $file
}

###
proc process_contents_cnt {output_fname input_fname help_fname} {
    # lee el file
    set file [open $input_fname]
    set data [read $file]
    close $file
    # substituye
    regsub -all FILE.HLP $data $help_fname newdata
    # crea el file de salida read only
    if [file exist $output_fname] { file delete $output_fname }
    set file [open $output_fname w 0444]
    puts $file $newdata
    close $file
}

###
proc create_zipse {zipCmd outDir tmpDir zipFname iconFname infFname fileName \
	verMajor verMinor verRelease codeName legalStr} {
    # lee el build
    set file [open lastbuild]; gets $file build; close $file
    # crea el archivo para el splash
    set dialogText "$codeName $verMajor.$verMinor.$verRelease Build: $build

$legalStr

por informacion visite http://www.rodablo.com/dbo"
    set dialogFname [file join $tmpDir dialog]
    set file [open $dialogFname w]; puts $file $dialogText; close $file
    # crea el file de opciones
    set optionText "
-win32
-i $iconFname
-st \"Instalador del $codeName $verMajor.$verMinor.$verRelease Build: $build\"
-a [file native $dialogFname]
-t [file native $dialogFname]
-setup
-le
-o
-c \"rundll32 setupapi,InstallHinfSection DI 132 .\\$infFname\"
"
    set optionFname [file join $tmpDir option]
    set file [open $optionFname w]; puts $file $optionText; close $file
    # invoca al winzipse
    exec $zipCmd $zipFname @[file native $optionFname]
    # renombra (el winzip lo deje en el tmp)
    file rename -force \
	    [file join $tmpDir $fileName.exe] \
	    [file join $outDir dbo-$verMajor-$verMinor-$verRelease-$build.exe]   
    # clean-up
    file delete -force $dialogFname $optionFname
#    exit $errorlevel
}

###
proc debug_reg_server {action progId classId libId verMajor verMinor dll} {
    package require registry 1.0 
    set dllPath [file native [file join [pwd] $dll]]
    switch -exact -- $action {
	register {
	    #
	    set key "HKEY_CLASSES_ROOT\\$progId.Session"
	    registry set $key "" "$progId debug reg!"
	    registry set "$key\\CLSID" "" "\{$classId\}"
	    #
	    set key "HKEY_CLASSES_ROOT\\CLSID\\\{$classId\}"
	    registry set $key "" "$progId debug reg!"
	    registry set "$key\\ProgID" "" "$progId.Session"
	    registry set "$key\\TypeLib" "" "\{$libId\}"
	    registry set "$key\\Version" "" "$verMajor.$verMinor"
	    registry set "$key\\InProcServer32" "" $dllPath
	    registry set "$key\\InProcServer32" "ThreadingModel" "Both"
	    registry set "$key\\Programmable"
	    #
	    set key "HKEY_CLASSES_ROOT\\TypeLib\\\{$libId\}"
	    registry set "$key\\$verMajor.$verMinor" "" "$progId debug reg!"
	    registry set "$key\\$verMajor.$verMinor\\0\\win32" "" $dllPath
	    registry set "$key\\$verMajor.$verMinor\\FLAGS" "" "0"
	}
	unregister {
	    registry delete "HKEY_CLASSES_ROOT\\$progId.Session"
	    registry delete "HKEY_CLASSES_ROOT\\CLSID\\\{$classId\}"
	    registry delete "HKEY_CLASSES_ROOT\\TypeLib\\\{$libId\}"
	}
    }
    puts "$dllPath ${action}ed!"
}

###
### ARRANCA AQUI
###
switch -exact -- [lindex $argv 0] {
    --process-versioninfo {
	eval "process_versioninfo [lrange $argv 1 2]"
    }
    --process-dboinf {
	eval "process_dboinf [lrange $argv 1 2]"
    }
    --process-contents.cnt {
	eval "process_contents_cnt [lrange $argv 1 3]"
    }
    --create-zipse {
	eval "create_zipse [lrange $argv 1 12]"
    }
    --debug-reg-server {
	eval "debug_reg_server [lrange $argv 1 7]"
    }
}





# Visual Tcl v1.2 Project
# todo: remove when done

##############################################################################################
#
# Notice:
# This document contains information that is proprietary to RADVISION LTD..
# No part of this publication may be reproduced in any form whatsoever without
# written prior approval by RADVISION LTD..
#
# RADVISION LTD. reserves the right to revise this publication and make changes
# without obligation to notify any person of such revisions or changes.
#
##############################################################################################

##############################################################################################
#                                 TAP_testapp.tcl
#
#  This is the main file of the graphic user interface of test application.
#  The file includes all the windows definitions.
#  The file is written in Tcl script and can be used on any platform.
#
#
#
#       Written by                        Version & Date                        Change
#      ------------                       ---------------                      --------
#  Oren Libis & Ran Arad                    04-Mar-2000
#
##############################################################################################


# Application veriable
# This variable holds all the information about the TCL application
global tmp app



##############################################################################################
#
#   INIT operations
#
##############################################################################################


# init:setGlobals
# Initialize the global variables in the application.
# input  : none
# output : none
# return : none
proc init:setGlobals {} {
    global tmp app tcl_platform

    set app(disableH450) 0
    set app(disableSEC) 0
    set app(disableRTP) 0
    set app(disableLDAP) 1
    set app(disableH460) 0

    set tmp(tvars) {}

    # Set external editors according to the operating system
    if {$tcl_platform(platform) == "unix"} {
        set app(editor,log)     "xemacs"
        set app(editor,config)  "xemacs"
        set app(editor,script)  "xemacs"
    }
    if {$tcl_platform(platform) == "windows"} {
        set app(editor,log)     "notepad.exe"
        set app(editor,config)  "c:\\Program Files\\Windows NT\\Accessories\\Wordpad.exe"
        set app(editor,script)  "notepad.exe"
    }

    # Set default window sizes and positions
    set app(test,size) 570x400+50+40
    set app(status,size) 425x280+81+135
    set app(log,size) 425x280+81+135
    set app(incall,size) 425x280+81+135
    set app(redial,size) 400x390+200+120
    set app(newchan,size) 400x390+200+120

    set tmp(maxLines) 1000
    set tmp(totalChannels) 0

    #Set default values of several stack parameters
    set app(options,currTab) 1
    set app(options,replayMedia) 1
    set app(options,nonStandardData) ""
    set app(options,userUser) "RADVISION"
    set app(options,display) "Stack Test Application"
    set app(options,autoAccept) 1
    set app(options,autoDrop) 1
    set app(options,autoAcceptFs) 1
    set app(options,autoMimic) 1
    set tmp(options,scriptMode) 0
    set app(options,suppressMessages) 0
    set app(options,scriptLogs) 0
    set app(options,popUp) 1
    set app(options,dialFrom) ""
    set app(sec,userPrime) 0

    # Log window parameters
    set tmp(log,messages) {}
    set tmp(log,cleaning) 0
    set tmp(log,cleanInterval) 5000

    # Status window parameters
    set tmp(status,isRunning) 0
    set tmp(status,warning) 90
    set tmp(status,isSet) 0
    set tmp(status,check) -1

    # Default help window parameters
    set app(help,currTab) 1

    set app(newcall,maxRate) 1280
    set tmp(faststart,channels) 10
    set tmp(partialURQ) 0
    set app(test,currTab) 1
    set app(h450,action) ""
    set app(h450,fwdrrt) 0
    set app(h450,indication) 0
    set app(h450,offer) 0
    set app(h450,completion) 0
    set app(suppress,releaseComplete) 0
    set app(suppress,endSessionCommand) 0

    # Toplevel windows we're interested in their geometry size
    set tmp(tops) { "test" "options" "status" "log" "incall" "redial" "inchan" }

    # Application parameters to save
    set tmp(testapp,varFile) "appOptionsFile.ini"

    set tmp(testapp,lists) { "newcall,ip" "rgrq,ip" "facility,ip" }

    # Set start time of the application
    set tmp(testapp,start) [clock seconds]

    # We haven't updated the gui yet
    set tmp(updatedGui) ""
    set tmp(recorder) 0
    set tmp(record,cnt,conn,in) 0
    set tmp(record,cnt,conn,out) 0
    set tmp(record,cnt,compService) 0

    set tmp(faststart,delayList) ""
    set tmp(faststart,accept) 0

    set app(h450,defAns) DefaultAck
    set app(h450,disableWait) 0
    set app(h450,nameType) None
    set app(h450,nameID) ""
    set app(h450,enablePrk) 1
    set app(h450,alertingPrk) 0
    set app(h450,groupList) ""
    set app(h450,retainServ) 1
    set app(h450,retainConn) 1
    set app(h450,compBusy) 0
    set app(h450,compFoundService) "<None>"
    set app(h450,intrPossible) 1
    set app(h450,intrIsIsolated) 0

    set tmp(nsd,str) ""

    trace variable app(options,scriptLogs) w init:traceLinkedVars
}


# init:traceLinkedVars
# This procedure traces linked variables to allow linking array elements to a C variable
# input  : name1    - Array name (app)
#          name2    - Element name in array
#          op       - Operation on variable
# output : none
# return : none
proc init:traceLinkedVars {name1 name2 op} {
    global tmp app

    scan $name2 {%*[^,],%s} varName

    global $varName
    set $varName $app($name2)
}


# init:loadSourceFiles
# Load the TCL source files of the project
# input  : files    - The list of files to load
# output : none
# return : none
proc init:loadSourceFiles {files} {

    foreach fileName $files {
        if {[file exists $fileName] == 1} {
            source $fileName
        } else {
            tk_dialog .err "Error" "Couldn't find $fileName" "" 0 "Ok"
        }
    }
}


# init:gui
# Initialize the GUI of the main window and open it
# input  : none
# output : none
# return : none
proc init:gui {} {
    global tmp

    font create ms -family "ms sans serif" -size 8

    # Make sure both Windows and Unix platforms display the same options
    gui:SetDefaults

    # rename all button widgets to allow adding the tooltip option
    gui:ToolTipWidgets

    # Create the images needed by the application
    image create bitmap fileDown -data "#define down_width 9 #define down_height 7
        static unsigned char down_bits[] = {
            0x00, 0x00, 0xfe, 0x00, 0xfe, 0x00, 0x7c, 0x00, 0x38, 0x00, 0x10, 0x00,  0x00, 0x00
        };"
    image create bitmap bmpClear -data "#define down_width 8 #define down_height 8
        static unsigned char down_bits[] = {
            0x00, 0x63, 0x36, 0x1c, 0x1c, 0x36, 0x63, 0x00
        };"
    image create photo ConfigBut -data $tmp(config)
    image create photo OptionsBut -data $tmp(options)
    image create photo StatusBut -data $tmp(status)
    image create photo RtpInfoBut -data $tmp(rtp)
    image create photo LogBut -data $tmp(log)
    image create photo LogFileBut -data $tmp(logfile)
    image create photo RaiseBut -data $tmp(raise)
    image create photo ExecuteBut -data $tmp(script)
    image create photo StopBut -data $tmp(stopscript)
    image create photo picExclamation -data $tmp(exclaim)
    image create photo picInformation -data $tmp(info)
    image create photo sunkX -data $tmp(sunkX)
    image create photo sunkUp -data $tmp(sunkUp)
    image create photo sunkDown -data $tmp(sunkDown)
    image create photo sunkSlash -data $tmp(sunkSlash)
    image create photo montage_br -data $tmp(montage_br)
    image create photo montage_bl -data $tmp(montage_bl)
    image create photo montage_tr -data $tmp(montage_tr)
    image create photo montage_tl -data $tmp(montage_tl)

    dummy:create
    wm overrideredirect .dummy 1

    Window open .test

    # this bind is for using mouse wheel inside listbox, textbox and canvas
    bind all <MouseWheel> "+gui:WheelEvent %X %Y %D"
}


# init:logger
# Initialize the Logger of the test application
# input  : none
# output : none
# return : none
proc init:logger {} {
    global tmp app

    set tmp(logFid) -1

    Log.Update $app(options,catchStackLog)
}



# init
# This procedure is the main initialization proceudre
# It is called during startup and it is responsible for the whole initialization process.
# input  : none
# output : none
# return : none
proc init {} {
    global tmp app

    init:loadSourceFiles { "TAP_splash.tcl" }
    test:splash

    # Make sure we're running from the test application
    set i [catch {set copyVersion $tmp(version)}]
    if { $i != 0 } {
        set tmp(version) "Test Application: TCL ONLY"
        set noStack 1
    } else {
        unset copyVersion
        set noStack 0
    }

    init:setGlobals
    init:loadSourceFiles {
        "TAP_balloon.tcl"
        "TAP_images.tcl"
        "TAP_pics_tr.tcl"
        "TAP_pics_tl.tcl"
        "TAP_pics_br.tcl"
        "TAP_pics_bl.tcl"
        "TAP_test.tcl"
        "TAP_help.tcl"
        "TAP_h450.tcl"
        "TAP_tools.tcl"
        "TAP_calls.tcl"
        "TAP_channels.tcl"
        "TAP_ras.tcl"
        "TAP_wrapper.tcl"
        "TAP_gui.tcl"
    }
    if {[file exists "TAP_recorder.tcl"] == 1} {
        set tmp(recorder) 1
        source "TAP_recorder.tcl"
    }

    init:LoadOptions

    init:gui

    if {$noStack == 1} {
        # Get stack stub functions and then display the GUI
        source "TAP_stub.tcl"
    }

    init:logger
}



# dummy:create
# This procedure creates a dummy top level window.
# We use this window to group all transient windows below it - this allows us to display the test
# application's main window on top of its transient windows.
# input  : none
# output : none
# return : none
proc dummy:create {} {

    ###################
    # CREATING WIDGETS
    ###################
    toplevel .dummy -class Toplevel
    wm focusmodel .dummy passive
    wm geometry .dummy 0x0+5000+0
    wm maxsize .dummy 0 0
    wm minsize .dummy 0 0
    wm overrideredirect .dummy 0
    wm resizable .dummy 0 0
    wm deiconify .dummy

    # dummy window should have no title, otherwise the testapp will show up twice in the task manager
    wm title .dummy ""
}



# init:SaveOptions
# This procedure saves values of option parameters in the application
# input  : displayMessage   - Indicate if we have to display a message when we finish
# output : none
# return : none
proc init:SaveOptions {displayMessage} {
    global tmp app

    set varFile [open $tmp(testapp,varFile) w+]

    # Save normal variables
    foreach varName [lsort [array names app]] {
        if {[info exists app($varName)] == 1} {
            set value "$app($varName)"
            #eval ".test.msg.list insert end $value"
            eval "puts $varFile {app($varName)=$value}"
        }
    }

    # Save lists
    foreach varName $tmp(testapp,lists) {
        set base "tmp($varName,"
        set ok 1
        set index 0

        while {$ok} {
            set varName "$base$index)"
            set ok [info exists $varName]

            if {$ok} {
                set value "$$varName"
                eval "puts $varFile $varName=$value"
            }
            incr index
        }
    }

    close $varFile

    if {$displayMessage == 1} {
        msgbox "Saved" picInformation "Application options saved" { { "Ok" -1 <Key-Return> } }
    }

    # ignore the temporary status checks
    set tmp(status,isSet) 0
}


# init:LoadOptions
# This procedure loads values of option parameters in the application
# input  : none
# output : none
# return : none
proc init:LoadOptions {} {
    global tmp app

    if {[file exists $tmp(testapp,varFile)] != 1} return

    set varFile [open $tmp(testapp,varFile) r+]

    while {[eof $varFile] != "1"} {
        # Read line from configuration file
        gets $varFile line

        # Parse it to parameter and value
        set equalIndex [string first "=" "$line"]
        set varName [string range $line 0 [expr {$equalIndex - 1}]]
        set value [string range $line [expr {$equalIndex + 1}] end]

        if {$varName != ""} {
            # Set the value
            eval "set $varName {$value}"
        }
    }

    close $varFile
}

proc test:splash {} {
    global tmp app

    set sw [winfo screenwidth .]
    set sh [winfo screenheight .]

    ###################
    # CREATING WIDGETS
    ###################
    wm focusmodel . passive
    wm overrideredirect . 1
    wm resizable . 0 0
    wm deiconify .
    wm title . "RADVISION Ensemble Product"

    image create photo splash -format gif -data $tmp(splash)

    label .pic -image splash -borderwidth 0
    grid rowconf . 0 -weight 1
    grid columnconf . 0 -weight 1
    grid .pic -in . -column 0 -row 0 -sticky nwse

    set w [image width splash]
    set h [image height splash]

    set x [expr ($sw - $w) / 2]
    set y [expr ($sh - $h) / 2]
    wm geometry . ${w}x$h+$x+$y

    update
}

init

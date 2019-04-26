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
#                                 TAP_tools.tcl
#
#   Tools menu handling.
#   This file holds all the GUI procedures for the Tools menu. This includes the log window,
#   resources status information and the default options of the stack.
#
#
#       Written by                        Version & Date                        Change
#      ------------                       ---------------                      --------
#  Oren Libis & Ran Arad                    04-Mar-2000
#
##############################################################################################



# options:chooseEditor
# Browse filelist to select an external editor program
# input  : type - Type of file being edited
# output : none
# return : none
proc options:chooseEditor {type} {
    global app

    set ed [tk_getOpenFile -filetypes {{ "Programs" {*.exe} }} -title "Select editor"]

    if {($ed != "") && [file exists $ed]} {
        set app(editor,$type) $ed
    }
}

##############################################################################################
#
#   LOG operations
#
##############################################################################################


# log:create
# Creation procedure of the Log window
# This window is opened when Tools|Log is selected from the main menu
# input  : none
# output : none
# return : none
proc log:create {} {
    global tmp app

    test:activateTool log

    if {[winfo exists .log]} {
        wm deiconify .log
        return
    }

    ###################
    # CREATING WIDGETS
    ###################
    toplevel .log -class Toplevel
    wm focusmodel .log passive
    wm geometry .log 350x250+350+400
    wm maxsize .log 1028 753
    wm minsize .log 104 1
    wm overrideredirect .log 0
    wm resizable .log 1 1
    wm deiconify .log
    wm title .log "Application Log"
    wm protocol .log WM_DELETE_WINDOW {Window close .log;focus .test}
    wm transient .log .dummy

    frame .log.main \
        -borderwidth 2 -height 75 -relief groove -width 125
    frame .log.main.filters \
        -relief groove
    checkbutton .log.main.filters.bug \
        -borderwidth 0 -text Bug \
        -variable log.bug -command {Log.SetFilter B ${log.bug} ${log.filter}}
    checkbutton .log.main.filters.fatal \
        -borderwidth 0 -text Fatal \
        -variable log.fatal -command {Log.SetFilter F ${log.fatal} ${log.filter}}
    checkbutton .log.main.filters.inf \
        -borderwidth 0 -text Info \
        -variable log.inf -command {Log.SetFilter I ${log.inf} ${log.filter}}
    checkbutton .log.main.filters.detailed \
        -borderwidth 0 -text Detailed \
        -variable log.detailed -command {Log.SetFilter D ${log.detailed} ${log.filter}}
    checkbutton .log.main.filters.warning \
        -borderwidth 0 -text Warning \
        -variable log.warning -command {Log.SetFilter W ${log.warning} ${log.filter}}
    menubutton .log.main.filters.type \
        -height 1 -indicatoron 1 \
        -menu .log.main.filters.type.01 -pady 2 -relief raised \
        -text "<all>" -textvariable log.filter -width 9
    menu .log.main.filters.type.01 \
        -activeborderwidth 1 -tearoff 0

    set filters {
        "<all>" ASN CFG CM H245 H245MSG HASH LI LOCK NETMGR PER Q931 Q931MSG
        RA RAS RASMSG RGS RPOOL SCRIPT SELI TIMER TPKT
    }

    foreach filter $filters {
        .log.main.filters.type.01 add radiobutton -indicatoron 0 -value $filter \
            -variable log.filter -label $filter -command "Log.FetchFilter $filter"
    }

    frame .log.main.control -borderwidth 2
    entry .log.main.control.maxln \
        -textvariable tmp(maxLines) -width 10 -validate key -vcmd {string is digit %P} -invcmd bell
    label .log.main.control.mlnlab -text {Max Lines}
    button .log.main.control.clear \
        -text Clear \
        -command {.log.main.log delete 0 end}
    listbox .log.main.log -yscrollcommand ".log.main.yscr set" -xscrollcommand ".log.main.xscr set" \
        -background White -listvar tmp(log,messages)
    scrollbar .log.main.yscr -command ".log.main.log yview"
    scrollbar .log.main.xscr -orient horizontal -command ".log.main.log xview"

    ###################
    # SETTING GEOMETRY
    ###################
    grid columnconf .log 0 -weight 1
    grid rowconf .log 0 -weight 1
    grid .log.main \
        -in .log -column 0 -row 0 -sticky nesw
    grid columnconf .log.main 0 -weight 1
    grid rowconf .log.main 1 -weight 1
    grid .log.main.filters -in .log.main -column 0 -row 0 -columnspan 2 -sticky ew
    grid columnconf .log.main.filters 0 -weight 3
    grid columnconf .log.main.filters 1 -weight 1
    grid columnconf .log.main.filters 2 -weight 1
    grid columnconf .log.main.filters 3 -weight 1
    grid columnconf .log.main.filters 4 -weight 1
    grid columnconf .log.main.filters 5 -weight 1
    grid .log.main.filters.bug -in .log.main.filters -column 4 -row 0
    grid .log.main.filters.fatal -in .log.main.filters -column 3 -row 0
    grid .log.main.filters.inf -in .log.main.filters -column 2 -row 0
    grid .log.main.filters.detailed -in .log.main.filters -column 1 -row 0
    grid .log.main.filters.warning -in .log.main.filters -column 5 -row 0
    grid .log.main.filters.type -in .log.main.filters -column 0 -row 0 -padx 3 -sticky ew
    grid .log.main.control -in .log.main -column 0 -row 4 -columnspan 2 -sticky ew
    grid columnconf .log.main.control 2 -weight 1
    grid .log.main.control.maxln -in .log.main.control -column 1 -row 0 -ipady 1
    grid .log.main.control.mlnlab \
        -in .log.main.control -column 0 -row 0 -padx 3
    grid .log.main.control.clear -in .log.main.control -column 1 -row 0 -columnspan 2 -sticky e
    grid .log.main.log -in .log.main -column 0 -row 1 -sticky nesw
    grid .log.main.yscr -in .log.main -column 1 -row 1 -sticky ns
    grid .log.main.xscr -in .log.main -column 0 -row 2 -sticky we

    ###########
    # BINDINGS
    ###########
    bind .log.main.log <<ListboxSelect>> {.log.main.log selection clear 0 end}

    ###################
    # OTHERS
    ###################
    Log.FetchFilter "<all>"
}

# Log:Write
# Log a message from the application
# input  : message  - The message to log
# output : none
# return : none
proc Log:Write {message} {
    global tmp

    # Make sure we create and open the logfile if necessary
    if {$tmp(logFid) == -1} {
        set tmp(logFid) [open "testapp.log" "w"]
    }

    puts $tmp(logFid) "$message"
    Log:Insert "$message\n"
}


# Log:Insert
# Add a message to the log window
# input  : message  - The message to log
# output : none
# return : none
proc Log:Insert {message} {
    global tmp

    lappend tmp(log,messages) $message

    if { ($tmp(log,cleaning) == 0) && ($tmp(maxLines) < [llength tmp(log,messages)])} {
        # Set a cleaning operation on the log...
        set tmp(log,cleaning) 1
        after idle log:cleanMessages
    }

    # Make sure the added line is displayed on screen
    catch {.log.main.log see end}
}

# log:cleanMessages
# Delete messages if there are too many when we're idle
# input  : none
# output : none
# return : none
proc log:cleanMessages {} {
    global tmp

    # remove the excessive lines
    lreplace tmp(log,messages) 0 [expr [llength tmp(log,messages)] - $tmp(maxLines)]

    set tmp(log,cleaning) 0
}

# log:clear
# This procedure is called when the log window is closed
# It allows the tool on the main window to be deactivated
# input  : none
# output : none
# return : none
proc log:clear {} {
    test:deactivateTool log
}


##############################################################################################
#
#   STATUS operations
#
##############################################################################################


# status:create
# Creation procedure of the Status window
# This window is opened when Tools|Status is selected from the main menu
# input  : none
# output : none
# return : none
proc status:create {} {
    global tmp app

    test:activateTool status

    # Make sure we set the status callback event
    set tmp(status,isRunning) 1

    if {[winfo exists .status]} {
        wm deiconify .status
        set tmp(status,id) [after idle status:elapsedTime]
        return
    }

    ###################
    # CREATING WIDGETS
    ###################
    toplevel .status -class Toplevel
    wm focusmodel .status passive
    wm geometry .status 453x412+200+236
    wm maxsize .status 1028 753
    wm minsize .status 104 1
    wm overrideredirect .status 0
    wm resizable .status 1 1
    wm deiconify .status
    wm title .status "Status"
    wm protocol .status WM_DELETE_WINDOW { .status.closeBut invoke }
    wm transient .status .dummy

    button .status.closeBut \
        -text Close \
        -command {Window close .status;focus .test}

    label .status.labBlue
    foreach column {module resource start cur curUsage max} \
            width {6 14 8 8 8 8} \
            header {MODULE RESOURCE START CUR USAGE MAX} \
    {
        label .status.lab$column -text $header
        listbox .status.$column -width $width -background White -yscrollcommand ".status.yscr set"
    }

    canvas .status.graphAll -borderwidth 2 -relief sunken -height 50 -background black
    canvas .status.graphOne -borderwidth 2 -relief sunken -height 50 -background black

    scrollbar .status.yscr -command "status:yviewScrl"

    ###################
    # SETTING GEOMETRY
    ###################
    grid rowconf .status 1 -weight 1
    grid .status.closeBut -in .status -column 0 -row 3 -columnspan 6 -padx 2 -pady 2 -sticky e

    foreach column {module resource start cur curUsage max} i {0 1 2 3 4 5} {
        grid columnconf .status $i -weight 1
        grid .status.lab$column -in .status -column $i -row 0 -sticky nesw
        grid .status.$column -in .status -column $i -row 1 -sticky nesw
    }

    grid .status.graphAll -in .status -column 0 -row 2 -columnspan 3 -sticky nesw
    grid .status.graphOne -in .status -column 3 -row 2 -columnspan 4 -sticky nesw

    grid .status.labBlue -in .status -column 6 -row 0 -sticky nesw
    grid .status.yscr -in .status -column 6 -row 1 -sticky ns

    ###########
    # BINDINGS
    ###########
    foreach column {module resource start cur curUsage max} {
        bind .status.$column <<ListboxSelect>> ".status.$column selection clear 0 end"
    }

    ########
    # OTHER
    ########
    update
    set tmp(status,id) [after idle status:elapsedTime]
}

# status:yviewScrl
# Set the listboxes view according to the scrollbar changes
# input  : args - Arguments added to the procedure when a scrollbar event occurs
# output : none
# return : none
proc status:yviewScrl {args} {
    foreach column {module resource start cur curUsage max} {
        eval ".status.$column yview $args"
    }
}


# status:elapsedTime
# This procedure is called every second when the status window is opened.
# It allows refreshing the information inside the status window.
# input  : none
# output : none
# return : none
proc status:elapsedTime {} {
    global tmp app Script

    # Check if status window exists
    set exists [winfo exists .status]
    if {$exists} {set exists [winfo ismapped .status]}

    if { $exists } {
        # Remember the listbox's position
        set fraction [lindex [.status.module yview] 0]

        # Make sure we update window
        Status.Display
        status:InsertLine {} "Tcl/Tk var" {} {} [expr [array size app]+[array size tmp]] {}
        status:InsertLine {} "Tcl/Tk globals" {} {} [llength [info globals]] {}
        status:InsertLine {} "Tcl/Tk script" {} {} [array size Script] {}
        status:InsertLine {} "Tcl/Tk timers" {} {} [llength [after info]] {}

        # Set the listbox's position
        status:yviewScrl moveto $fraction

        # Call the same function in 1 second time to refresh it
        set tmp(status,id) [after 1000 {status:elapsedTime}]
    }
}


# status:clear
# This procedure is called when the status window is closed
# It allows removing the status:elapsedTime procedure from the event loop
# input  : none
# output : none
# return : none
proc status:clear {} {
    global tmp

    if {$tmp(status,isRunning) == 1} {
        set tmp(status,isRunning) 0
        after cancel $tmp(status,id)
    }

    test:deactivateTool status
}


# status:Clean
# This procedure cleans the information inside the status window
# input  : none
# output : none
# return : none
proc status:Clean {} {
    .status.module delete 0 end
    .status.resource delete 0 end
    .status.start delete 0 end
    .status.cur delete 0 end
    .status.curUsage delete 0 end
    .status.max delete 0 end
}

# status:InsertLine
# This procedure inserts a line into the status window
# input  : module   - Module's name
#          resource - Resource's name
#          ini      - Initial resource allocation value
#          cur      - Current resource allocation value
#          maxUsage - Maximum resource allocations used in the execution of the application
#          max      - Maximum available resource allocations
# output : none
# return : none
proc status:InsertLine {module resource ini cur maxUsage max} {
    global tmp app

    # Make sure we set the module and resource name properly
    .status.module insert end $module
    .status.resource insert end $resource

    # Add the indications
    .status.start insert end $ini
    .status.cur insert end $cur
    .status.curUsage insert end $maxUsage
    .status.max insert end $max

    # See if we should color the information in some way
    if {($cur != {}) && ($max != {}) && ($max != 0) && (($cur * 100) / $max >= $tmp(status,warning))} {
        .status.resource itemconfigure end -foreground red
        .status.cur itemconfigure end -foreground red
    }
}


# status:DrawGraphs
# This procedure draws the status graphs on the screen
# input  : none
# output : none
# return : none
proc status:DrawGraphs {} {

    # Clear the canvas
    .status.graphAll delete all

    set x 10
    for {set i 0} {$i < [.status.resource size]} {incr i} {
        set cur [.status.cur get $i]
        set maxUsage [.status.curUsage get $i]
        set max [.status.max get $i]

        if {($max != "") && ($max != 0)} {
            .status.graphAll create rectangle $x 51 [expr $x+3] [expr 50-(40*$maxUsage)/$max] \
                -fill #006000 -outline #006000
            .status.graphAll create rectangle $x 51 [expr $x+3] [expr 50-(40*$cur)/$max] \
                -fill #00ff00 -outline #00ff00

            incr x 5
        }

    }
}

proc status:PreCheck {} {
    global tmp
    if {$tmp(status,isSet) == 1} {
        status:Check $tmp(status,tpkt) $tmp(status,messages) $tmp(status,pvt) $tmp(status,timers)
    }
}



##############################################################################################
#
#   CONFIG operations (Open config.val)
#
##############################################################################################


# config:open
# opens the config.val file to be edited buy the most common text editor in either
# windows (notepad) or unix (xemacs).
# Called from the options menu.
# input  : none
# output : none
# return : none
proc config:open {} {
    global tmp app

    exec $app(editor,config) $tmp(configFilename) > nul &

    test:activateTool config
    after 150 test:deactivateTool config
}



##############################################################################################
#
#   Log File operations (Open the current logfile)
#
##############################################################################################


# logfile:open
# opens the stacklog.txt file to be edited buy notepad. for now, does nothing under Unix
# Called from the ToolBar.
# input  : none
# output : none
# return : none
proc logfile:open {} {
    global tmp app tcl_platform

    if {$tcl_platform(platform) != "unix"} {
        exec $app(editor,log) "[api:ms:GetLogFilename]" < $app(editor,log) > nul &
    } else {
        exec $app(editor,log) "[api:ms:GetLogFilename]" &
    }

    test:activateTool logFile
    after 150 test:deactivateTool logFile
}





##############################################################################################
#
#   MSGBOX operations
#
##############################################################################################


# msgbox
# Create a message box on screen, displaying information and waiting for user to reply
# input  : title    - Title to display for the message box
#          picture  - Picture to display near the message - can be left empty
#          msg      - Message to display inside the message box
#          buttons  - List of buttons to pop.
#                     Each button is a list of 3 elements:
#                     {string underlinePosition key-binding}
# output : none
# return : the selected button's string
proc msgbox {title picture msg buttons} {
    global tmp app tcl_platform

    set lastWindow [focus]
    set tmp(msgbox,result) ""

    ###################
    # CREATING WIDGETS
    ###################
    toplevel .msgbox -class Toplevel
    wm transient .msgbox .dummy
    wm resizable .msgbox 0 0
    wm title .msgbox "$title"
    wm protocol .msgbox WM_DELETE_WINDOW { focus .test }

    frame .msgbox.msg -borderwidth 2 -relief groove
    label .msgbox.msg.text -borderwidth 0 -relief flat -text $msg -justify left -font {{} 12 {}}

    frame .msgbox.buttons -relief sunken
    foreach i $buttons {
        # Get each button's information from the list
        set btnText [lindex $i 0]
        set underline [lindex $i 1]
        set bindKey [lindex $i 2]

        # Create the widget with the command and grid it in
        set widget [string tolower $btnText]
        button .msgbox.buttons.$widget -text $btnText -width 5 -underline $underline \
            -command "set tmp(msgbox,result) \"$btnText\"; destroy .msgbox" -font {{} 14 {}}
        pack .msgbox.buttons.$widget -side left -expand 1 -fill x
        bind .msgbox $bindKey ".msgbox.buttons.$widget invoke"
    }

    if {$picture != ""} {
        label .msgbox.msg.picture -relief flat -border 1 -image $picture
        grid .msgbox.msg.picture -in .msgbox.msg -row 0 -column 0
    }

    ###################
    # SETTING GEOMETRY
    ###################
    grid columnconf .msgbox 0 -weight 1
    grid rowconf .msgbox 0 -weight 1

    grid .msgbox.msg -in .msgbox -row 0 -column 0 -ipadx 5 -ipady 5 -sticky nesw
    grid columnconf .msgbox.msg 1 -weight 1
    grid rowconf .msgbox.msg 0 -weight 1
    grid .msgbox.msg.text -in .msgbox.msg -row 0 -column 1 -sticky nesw

    grid .msgbox.buttons -in .msgbox -row 1 -column 0 -padx 2 -pady 2 -sticky nesw

    #######################
    # CALCULATING GEOMETRY
    #######################
    wm withdraw .msgbox
    update idletasks
    set sw [winfo screenwidth .]
    set sh [winfo screenheight .]
    set w [winfo reqwidth .msgbox]
    set h [winfo reqheight .msgbox]
    set x [expr ($sw - $w) / 2]
    set y [expr ($sh - $h) / 2]
    if {$tcl_platform(platform) != "unix"} {
        wm deiconify .msgbox
    }
    wm geometry .msgbox +$x+$y
    if {$tcl_platform(platform) == "unix"} {
        wm deiconify .msgbox
    }

    ##########
    # EXECUTE
    ##########
    grab .msgbox
    tkwait window .msgbox
    grab release .msgbox

    after idle "focus $lastWindow"
    return $tmp(msgbox,result)
}






##############################################################################################
#
#   NOTEBOOK operations
#
##############################################################################################


# notebook:create
# Create a notebook-like widget
# input  : var  - Variable group to use
#          base - Base container widget to use for the notebook created
#          tabs - List of tabs to use in notebook
#                 Each tab is a list of 2 elements:
#                 name  - String to use on tab's name
#                 cb    - Callback function that draws the tab's contents (cb <base_frame> <tab_button>)
# output : none
# return : name of the created notebook "widget"
proc notebook:create {var base tabs} {
    global tmp app

    set base $base.canv
    set numTabs [llength $tabs]
    set i 1

    canvas $base -borderwidth 0 -relief flat -width 1 -height 1

    foreach tab $tabs {
        set tabName [lindex $tab 0]
        set tabCallback [lindex $tab 1]

        # Create a tab header
        button $base.but$i -borderwidth 0 -relief flat -text $tabName -width 0 -padx 1 -pady 2 \
            -command "notebook:changeSelection $var $i"

        # Create a frame for each tab
        frame $base.tab$i -borderwidth 0 -relief flat -height 1 -width 1 -takefocus 0
        eval {$tabCallback $base.tab$i $base.but$i}

        # Grid the button for tab
        grid $base.but$i -in $base -column $i -row 1 -padx 5

        # Make sure the tab frame knows where it should be
        grid $base.tab$i -in $base -column 0 -row 2 -columnspan [expr $numTabs+2] \
            -sticky news -padx 10 -pady 2

        incr i
    }

    # Set some global grid information for the notebook's canvas
    grid rowconf $base 0 -minsize 6
    grid rowconf $base 2 -weight 1
    grid rowconf $base 3 -minsize 4
    grid columnconf $base 0 -minsize 10
    grid columnconf $base [expr $numTabs+1] -weight 1 -minsize 20

    # Raise the current tab inside canvas
    raise $base.tab$app($var,currTab)

    # Bind expose event to make sure we draw the tabs correct
    bind $base <Expose> "
        $base delete border
        $base delete tabs
        notebook:border $var
        notebook:draw $var
    "

    return $base
}



# notebook:changeSelection
# Change the selection between the notebook tabs inside the canvas
# input  : top  - Top level window name (without the point)
#          new  - New tab to use (number of tab to select)
# output : none
# return : none
proc notebook:changeSelection {top new} {
    global tmp app

    # Change the frame inside the options window
    raise .$top.canv.tab$new

    # Make sure we know which frame we're in
    set app($top,currTab) $new
    .$top.canv delete tabs
    notebook:draw $top
}


# notebook:border
# Draw the border lines of the notebook
# input  : top  - Top level window name (without the point)
# output : none
# return : none
proc notebook:border {top} {
    global tmp app

    set canv .$top.canv

    #################
    # CANVAS DRAWING
    #################
    set Left [expr { [winfo x $canv.tab$app($top,currTab)] - 2 }]
    set Left1 [expr $Left + 2]
    set Right [expr { [winfo x $canv.tab$app($top,currTab)] + [winfo width $canv.tab$app($top,currTab)] + 2 }]
    set Right1 [expr $Right - 2]
    set Top [expr { [winfo y $canv.tab$app($top,currTab)] - 2 }]
    set Top1 [expr $Top + 2]
    set Bottom [expr { [winfo y $canv.tab$app($top,currTab)] + [winfo height $canv.tab$app($top,currTab)] + 2 }]
    set Bottom1 [expr $Bottom - 2]
    # top - left sides line
    $canv create line  $Left $Bottom1  $Left $Top1  $Left1 $Top  $Right1 $Top            -fill white -tags border
    # bottom - right sides line
    $canv create line  $Right $Top1  $Right $Bottom1  $Right1 $Bottom  $Left1 $Bottom   -fill black -tags border
    # bottom - right sides line, one inside
    incr Left
    incr Left1 -1
    incr Right -1
    incr Bottom -1
    incr Top
    incr Top1 -1
    $canv create line  $Right $Top  $Right $Top1  $Right $Bottom1  $Right1 $Bottom  $Left1 $Bottom    -fill DarkGrey -tags border
}


# notebook:draw
# Draw the notebook when necessary
# input  : top  - Top level window name (without the point)
# output : none
# return : none
proc notebook:draw {top} {
    global tmp app

    set canv .$top.canv

    set Left [expr { [winfo x $canv.but$app($top,currTab)] - 4 }]
    set Left1 [expr $Left + 2]
    set Left0 [expr $Left - 2]
    set Right [expr { [winfo x $canv.but$app($top,currTab)] + [winfo width $canv.but$app($top,currTab)] + 4 }]
    set Right1 [expr $Right - 2]
    set Right0 [expr $Right + 2]
    set Top [expr { [winfo y $canv.but$app($top,currTab)] - 4 }]
    set Top1 [expr $Top + 2]
    set Bottom [expr { [winfo y $canv.tab$app($top,currTab)] - 2 }]
    set Bottom1 [expr $Bottom - 2]
    set canvFace [$canv cget -background]
    $canv create line  $Left0 $Bottom  $Right0 $Bottom                                           -fill $canvFace -tags tabs
    $canv create line  $Left0 $Bottom  $Left $Bottom1  $Left $Top1  $Left1 $Top  $Right1 $Top    -fill White -tags tabs
    $canv create line  $Right $Top1  $Right $Bottom1  $Right0 $Bottom                            -fill Black -tags tabs
    incr Right -1
    incr Right0 -1
    incr Top
    incr Top1 -1
    $canv create line $Right $Top  $Right $Top1  $Right $Bottom1  $Right0 $Bottom                -fill DarkGrey -tags tabs
    foreach tab $tmp($top,otherTabs) {
        if [expr {$tab != $app($top,currTab)}] {
            set Left [expr { [winfo x $canv.but$tab] - 2 }]
            set Left1 [expr $Left + 2]
            set Right [expr { [winfo x $canv.but$tab] + [winfo width $canv.but$tab] + 2 }]
            set Right1 [expr $Right - 2]
            set Top [expr { [winfo y $canv.but$tab] - 2 }]
            set Top1 [expr $Top + 2]
            $canv create line $Left $Bottom  $Left $Top1  $Left1 $Top  $Right1 $Top     -fill White -tags tabs
            $canv create line $Right $Top1  $Right $Bottom -fill Black -tags tabs
            incr Right -1
            incr Top
            $canv create line $Right $Top $Right $Bottom -fill DarkGrey -tags tabs
        }
    }
}


proc notebook:refresh {top} {
    set base .$top.canv
    $base delete border
    $base delete tabs
    notebook:border $top
    notebook:draw $top
}


##############################################################################################
#
#   RTP-INFO operations
#
##############################################################################################


# rtpInfo:create
# Creation procedure of the RTP information window
# This window is opened when Tools|rtpInfo is selected from the main menu
# input  : none
# output : none
# return : none
proc rtpInfo:create {} {
    global tmp app

    test:activateTool rtpInfo

    # Make sure we set the rtp information callback event
    set tmp(rtpInfo,isRunning) 1

    if {[winfo exists .rtpInfo]} {
        wm deiconify .rtpInfo
        set tmp(rtpInfo,id) [after idle rtpInfo:elapsedTime]
        return
    }

    ###################
    # CREATING WIDGETS
    ###################
    toplevel .rtpInfo -class Toplevel
    wm focusmodel .rtpInfo passive
    wm geometry .rtpInfo 453x412+220+256
    wm maxsize .rtpInfo 1028 753
    wm minsize .rtpInfo 104 1
    wm overrideredirect .rtpInfo 0
    wm resizable .rtpInfo 1 1
    wm deiconify .rtpInfo
    wm title .rtpInfo "RTP Information"
    wm protocol .rtpInfo WM_DELETE_WINDOW { .rtpInfo.closeBut invoke }
    wm transient .rtpInfo .dummy

    button .rtpInfo.closeBut \
        -text Close \
        -command {Window close .rtpInfo;focus .test}

    listbox .rtpInfo.data -background White -yscrollcommand ".rtpInfo.yscr set"
    scrollbar .rtpInfo.yscr -command ".rtpInfo.data yview"

    ###################
    # SETTING GEOMETRY
    ###################
    grid rowconf .rtpInfo 0 -weight 1
    grid .rtpInfo.closeBut -in .rtpInfo -column 0 -row 1 -padx 2 -pady 2 -sticky e

    grid columnconf .rtpInfo 0 -weight 1
    grid .rtpInfo.data -in .rtpInfo -column 0 -row 0 -sticky nesw
    grid .rtpInfo.yscr -in .rtpInfo -column 1 -row 0 -sticky ns

    ###########
    # BINDINGS
    ###########
    bind .rtpInfo.data <<ListboxSelect>> ".rtpInfo.data selection clear 0 end"

    ########
    # OTHER
    ########
    set tmp(rtpInfo,id) [after idle rtpInfo:elapsedTime]
}

# rtpInfo:elapsedTime
# This procedure is called every 10 seconds when the RTP information window is opened.
# It allows refreshing the information inside the RTP window.
# input  : none
# output : none
# return : none
proc rtpInfo:elapsedTime {} {
    global tmp app
    set jumpToEnd 0

    # Check if status window exists
    set exists [winfo exists .rtpInfo]
    if {$exists} {set exists [winfo ismapped .rtpInfo]}

    if { $exists } {
        # Remember the listbox's position
        if {[lindex [.rtpInfo.data yview] 1] == 1} {set jumpToEnd 1}

        # Make sure we update window
        rtpInfo.Display

        # Set the listbox's position
        if {$jumpToEnd == 1} {.rtpInfo.data see end}

        # Call the same function in 10 seconds time to refresh it
        set tmp(rtpInfo,id) [after 10000 {rtpInfo:elapsedTime}]
    }
}


# rtpInfo:clear
# This procedure is called when the RTP information window is closed
# It allows removing the rtpInfo:elapsedTime procedure from the event loop
# input  : none
# output : none
# return : none
proc rtpInfo:clear {} {
    global tmp

    if {$tmp(rtpInfo,isRunning) == 1} {
        set tmp(rtpInfo,isRunning) 0
        after cancel $tmp(rtpInfo,id)
    }

    test:deactivateTool rtpInfo
}


# rtpInfo:InsertLine
# This procedure inserts a line into the RTP information window
# input  : str  - Line to insert
# output : none
# return : none
proc rtpInfo:InsertLine {str} {
    .rtpInfo.data insert end $str
}


##############################################################################################
#
#   Disable Module
#
##############################################################################################

# disableModule:create
# Creation procedure of the Disable Module window
# This window is opened when Tools|Disable is selected from the main menu
# input  : none
# output : none
# return : none
proc disableModule:create {} {
    global tmp app

    if {[winfo exists .disable]} {
        wm deiconify .disable
        return
    }

    ###################
    # CREATING WIDGETS
    ###################
    toplevel .disable -class Toplevel
    wm focusmodel .disable passive
    wm geometry .disable +350+400
    wm overrideredirect .disable 0
    wm resizable .disable 0 0
    wm deiconify .disable
    wm title .disable "Disable Module - requires restart of the stack!"
    wm protocol .disable WM_DELETE_WINDOW {
        .disable.cancel invoke
        focus .test
    }
    wm transient .disable .dummy
    focus .dummy
    gui:centerWindow .disable .test

    set tmp(disableH450) $app(disableH450)
    set tmp(disableSEC) $app(disableSEC)
    set tmp(disableLDAP) $app(disableLDAP)
    set tmp(disableRTP) $app(disableRTP)
    set tmp(disableH460) $app(disableH460)

    foreach {y n f t} {
        0 disableH450 test.H450Supported  "Disable H.450 (Supplamentary Services)"
        1 disableSEC  test.SecSupported   "Disable H.235v2 (Security)"
        2 disableLDAP test.LdapSupported  "Disable LDAP (Directory services)"
        3 disableRTP  test.RtpSupported   "Disable RTP (Media loopback)"
        4 disableH460 test.H460Supported  "Disable H.460 (FW/NAT Traversal)"
    } {
        set support ![catch {eval $f}]
        checkbutton .disable.$n -text $t -variable app($n)
        if {($support == "") || ($support == "0")} {
            .disable.$n configure -state disabled
        }
        grid .disable.$n -in .disable -column 0 -row $y -columnspan 3 -sticky w
    }

    button .disable.restart -text "RESTART" -width 8 -command {
            Window delete .disable
            focus .test
            test.Restart
        }
    button .disable.cancel -text "Cancel" -width 8 -command {
            global app tmp
            set app(disableH450) $tmp(disableH450)
            set app(disableSEC) $tmp(disableSEC)
            set app(disableLDAP) $tmp(disableLDAP)
            set app(disableRTP) $tmp(disableRTP)
            set app(disableH460) $tmp(disableH460)
            Window delete .disable
            focus .test
        }

    ###################
    # SETTING GEOMETRY
    ###################
    grid rowconf .disable 5 -weight 1
    grid columnconf .disable 0 -weight 1

    grid .disable.restart -in .disable -column 2 -row 5 -padx 2 -pady 2
    grid .disable.cancel  -in .disable -column 1 -row 5 -padx 2 -pady 2
}

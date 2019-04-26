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
#                                 TAP_test.tcl
#
# This file contains the main window GUI along with some of its procedures, which are not
# related to any other specific window (such as calls, tools, etc).
#
#
#
#       Written by                        Version & Date                        Change
#      ------------                       ---------------                      --------
#  Oren Libis & Ran Arad                    04-Mar-2000
#
##############################################################################################



##############################################################################################
#
#   MAIN WINDOW operations
#
##############################################################################################


# test:create
# This procedure creates the main window of the test application
# input  : none
# output : none
# return : none
proc test:create {} {
    global tmp app

    ###################
    # CREATING WIDGETS
    ###################
    toplevel .test -class Toplevel -menu .test.main
    wm iconify .test
    wm focusmodel .test passive
    wm geometry .test $app(test,size)
    wm overrideredirect .test 0
    wm resizable .test 1 1
    wm title .test $tmp(version)
    wm protocol .test WM_DELETE_WINDOW {
        set app(test,size) [winfo geometry .test]
        test.Quit
    }

    # Create the menu for this window
    test:createMenu

    # Tool buttons
    # todo : add edit script tool
    frame .test.line1 -relief sunken -border 1 -height 2
    frame .test.tools
    button .test.tools.log -relief flat -border 1 -command {Window toggle .log} -image LogBut -state disabled -tooltip "Stack Log"
    button .test.tools.config -relief flat -border 1 -command {config:open} -image ConfigBut -tooltip "Open Configuration File"
    button .test.tools.logFile -relief flat -border 1 -command {logfile:open} -image LogFileBut -tooltip "Open Log File"
    button .test.tools.status -relief flat -border 1 -command {Window toggle .status} -image StatusBut -tooltip "Resource Status (Ctrl-s)"
    button .test.tools.rtpInfo -relief flat -border 1 -command {Window toggle .rtpInfo} -image RtpInfoBut -tooltip "RTP/RTCP Information"
    button .test.tools.raise -relief flat -border 1 -command {focus .dummy} -image RaiseBut -tooltip "Raise Windows"
    label .test.tools.marker1 -relief sunken -border 1
    button .test.tools.execute -relief flat -border 1 -command {script:load} -image ExecuteBut -tooltip "Execute an Advanced Script"
    button .test.tools.stop -relief flat -border 1 -command {script:stop} -image StopBut -tooltip "Stop executing a running script"
    set tools {{log config status rtpInfo raise logFile} {execute stop}}

    # top bar
    image create photo topbar -format gif -data $tmp(topbarFade)
    label .test.tools.topbar -image topbar -borderwidth 0 -anchor e
    frame .test.line2 -relief sunken -border 1 -height 2

    # Calls listbox
    proc callYviewScrl {args} {
        eval ".test.calls.list yview $args"
        eval ".test.calls.conf yview $args"
        eval ".test.calls.conn yview $args"
    }

    frame .test.calls -borderwidth 2 -relief groove
    listbox .test.calls.list -background White \
        -selectmode single -exportselection 0 -height 3 \
        -yscrollcommand {.test.calls.scrl set}
    listbox .test.calls.conf -background White \
        -selectmode single -exportselection 0 -height 3 -width 1\
        -yscrollcommand {.test.calls.scrl set} -tooltip "Conference"
    listbox .test.calls.conn -background White \
        -selectmode single -exportselection 0 -height 3 -width 1\
        -yscrollcommand {.test.calls.scrl set} -tooltip "Host"
    scrollbar .test.calls.scrl -command {callYviewScrl}

    # Channels listbox
    frame .test.chan -borderwidth 2 -relief groove
    channel:createChannelsInfo .test.chan 1

    # Tabs Section
    set tmp(test,otherTabs) { 1 2 3 4 5 6 7 8 }

    set tabs [
        notebook:create test .test {
            { "Basic"   test:basicTab  }
            { "Call"    test:callTab   }
            { "Channel" test:chanTab   }
            { "RAS"     test:rasTab    }
            { "Misc."   test:miscTab   }
            { "Options" test:optionTab }
            { "H.450"   test:h450Tab   }
            { "Sec."    test:secTab   }
        }
    ]

    # Messages
    frame .test.msg -borderwidth 2 -relief groove
    listbox .test.msg.list -selectmode single -exportselection 0 -height 1 -background White \
        -yscrollcommand {.test.msg.yscrl set} -xscrollcommand {.test.msg.xscrl set}
    scrollbar .test.msg.xscrl -orient horizontal -command {.test.msg.list xview}
    scrollbar .test.msg.yscrl -command {.test.msg.list yview}
    button .test.msg.clear -borderwidth 2 -command {.test.msg.list delete 0 end} \
        -image bmpClear -tooltip "Clear message box (Ctrl-m)"

    # Status bar
    frame .test.status -borderwidth 0 -relief flat
    label .test.status.calls -relief sunken -anchor w -tooltip "Number of current connected calls, Number of total calls connected"
    label .test.status.pvt -relief sunken -width 8 -tooltip "PVT resources used above startup values"
    label .test.status.h245 -relief sunken -width 12 -tooltip "H245 address passing Stage"
    label .test.status.gk -relief sunken -width 12 -tooltip "Gatekeeper registration status" -textvariable tmp(status,gk)
    label .test.status.stack -relief sunken -width 10 -tooltip "Stack's current execution status"
    label .test.status.mode -relief sunken -width 7 -tooltip "Mode of the application: Normal mode, or Script mode"
    frame .test.status.log -relief sunken -width 16
    label .test.status.log.data -borderwidth 0 -relief flat -width 15 -tooltip "Errors found while running the stack"
    button .test.status.log.reset -borderwidth 2 -command Log:Reset \
        -image bmpClear -tooltip "Reset error counters"

    label .test.status.timer -relief sunken -padx 3 -tooltip "Time passed since beginning of execution (in minutes)"

    # Manual frame
    frame .test.manual -relief groove -borderwidth 0

    ###################
    # SETTING GEOMETRY
    ###################
    grid columnconf .test 0 -weight 1
    grid columnconf .test 1 -weight 0 -minsize 275
    grid rowconf .test 3 -weight 0 -minsize 6
    grid rowconf .test 4 -weight 0 -minsize 135
    grid rowconf .test 5 -weight 0 -minsize 7
    grid rowconf .test 6 -weight 0 -minsize 135
    grid rowconf .test 7 -weight 0 -minsize 5
    grid rowconf .test 8 -weight 1

    # Displaying the tool buttons
    grid .test.line1 -in .test -column 0 -row 0 -columnspan 3 -sticky new -pady 1
    grid .test.tools -in .test -column 0 -row 0 -columnspan 3 -sticky ew -pady 4
    set first 1
    foreach toolGroup $tools {
        foreach tool $toolGroup {
            pack .test.tools.$tool -in .test.tools -ipadx 2 -side left
        }
        if {$first == 1} {
            pack .test.tools.marker1 -in .test.tools -pady 2 -padx 1 -side left -fill y
            set first 0
        }
    }

    # top bar
    pack .test.tools.topbar -in .test.tools -side right
    grid .test.line2 -in .test -column 0 -row 0 -columnspan 3 -sticky sew -pady 1

    # Calls
    grid .test.calls -in .test -column 0 -row 4 -sticky nesw -pady 2
    grid columnconf .test.calls 1 -weight 1
    grid rowconf .test.calls 0 -weight 0 -minsize 3
    grid rowconf .test.calls 1 -weight 1
    grid .test.calls.conf -in .test.calls -column 0 -row 1 -sticky nesw -pady 2
    grid .test.calls.list  -in .test.calls -column 1 -row 1 -sticky nesw -pady 2
    grid .test.calls.conn  -in .test.calls -column 2 -row 1 -sticky nesw -pady 2
    grid .test.calls.scrl  -in .test.calls -column 3 -row 1 -sticky ns -pady 2

    # Channels
    grid .test.chan -in .test -column 0 -row 6 -sticky nesw -padx 2

    # Tabs Section
    grid $tabs -in .test -column 1 -row 4 -rowspan 3 -sticky nesw -pady 2

    # Messages
    grid .test.msg -in .test -column 0 -row 8 -columnspan 3 -sticky nesw -pady 4 -padx 2
    grid columnconf .test.msg 0 -weight 1
    grid rowconf .test.msg 0 -weight 0 -minsize 5
    grid rowconf .test.msg 1 -weight 1
    grid .test.msg.list -in .test.msg -column 0 -row 1 -sticky nesw -padx 1 -pady 1
    grid .test.msg.xscrl -in .test.msg -column 0 -row 2 -sticky we
    grid .test.msg.yscrl -in .test.msg -column 1 -row 1 -sticky ns
    grid .test.msg.clear -in .test.msg -column 1 -row 2 -sticky news

    # Status bar
    grid .test.status -in .test -column 0 -row 9 -columnspan 3 -sticky ew -padx 1
    grid columnconf .test.status 0 -weight 1
    grid .test.status.calls -in .test.status -column 0 -row 0 -sticky ew -padx 1
    grid .test.status.pvt -in .test.status -column 1 -row 0 -sticky ew -padx 1
    grid .test.status.h245 -in .test.status -column 2 -row 0 -sticky ew -padx 1
    grid .test.status.gk -in .test.status -column 3 -row 0 -sticky ew -padx 1
    grid .test.status.stack -in .test.status -column 4 -row 0 -sticky ew -padx 1
    grid .test.status.mode -in .test.status -column 5 -row 0 -sticky ew -padx 1
    grid .test.status.log -in .test.status -column 6 -row 0 -padx 1
    grid .test.status.log.data -in .test.status.log -column 0 -row 0 -sticky ew
    grid .test.status.log.reset -in .test.status.log -column 1 -row 0
    grid .test.status.timer -in .test.status -column 7 -row 0 -sticky ew -padx 1

    ###########
    # BINDINGS
    ###########
    bind .test <Control-Key-m> {.test.msg.clear invoke}
    bind .test <Control-Key-s> {.test.tools.status invoke}
    bind .test <Control-Key-l> {.test.tools.logFile invoke}
    bind .test <Control-Key-f> { for {set i 0} {$i<10} {incr i} {Call.Make $tmp(quick,dest)} }
    bind .test <Control-Key-R> {LogFile:Reset}
    bind .test.calls.list <Double-Button-1> {call:doubleclickedCall}
    bind .test.msg.list <<ListboxSelect>> {.test.msg.list selection clear 0 end}
    bind .test.calls.conf <<ListboxSelect>> {.test.calls.conf selection clear 0 end}
    bind .test.calls.conn <<ListboxSelect>> {.test.calls.conn selection clear 0 end}

    foreach toolGroup $tools {
        foreach tool $toolGroup {
            bindtags .test.tools.$tool "[bindtags .test.tools.$tool] toolbar"
        }
    }

    ########
    # OTHER
    ########
    bind .test.calls.list <<ListboxSelect>> {+
        set item [selectedItem .test.calls.list]
        if {$item != ""} {Channel.DisplayChannelList $item}
    }
    test:SetCalls 0 0
    test:SetH245Status $app(h245,stage)
    test:updateTimer
    trace variable tmp(script,running) w test:refreshMenu
    set tmp(script,running) 0

    placeHeader .test.calls "Calls"
    placeHeader .test.msg "Messages"
    after idle {wm geometry .test $app(test,size)}
}

proc test:basicTab {base tabButton} {
    global tmp app
    set tmp(basicTab) $base

    # Quick Call
    frame $base.quick -borderwidth 2 -relief groove
    button $base.quick.make -text "Make" -highlightthickness 1 -width 6 -borderwidth 2 \
        -command {global tmp; Call.Make $tmp(quick,dest)} -tooltip "Make a call"
    histEnt:create $base.quick $base.quick.make quick,dest "Address or Alias to call (Return to Make)"

    # conference and connection
    frame $base.cnc -borderwidth 2 -relief groove
    checkbutton $base.cnc.useAdHoc -text "Add to conference" -variable tmp(adHoc,use) -tooltip "Add new call to conference"\
        -command {
            global tmp
            $tmp(basicTab).cnc.callAdHoc.01 delete 0 end
            if { $tmp(adHoc,use) } {
                set tmp(adHoc,call) "Select Call"
                foreach call [.test.calls.list get 0 end] {
                    $tmp(basicTab).cnc.callAdHoc.01 add radiobutton \
                        -indicatoron 0 -value $call -variable tmp(adHoc,call) -label $call
                }
            } else { set tmp(adHoc,call) "" }
        }
    menubutton $base.cnc.callAdHoc -height 1 -width 10 -anchor w -indicatoron 1 \
        -menu $base.cnc.callAdHoc.01 -relief raised -text "" -textvariable tmp(adHoc,call) -tooltip "Select call to add to"
    menu $base.cnc.callAdHoc.01 -activeborderwidth 1 -tearoff 0

    checkbutton $base.cnc.useMultiplex -text "Use same connection" \
        -variable tmp(multiplex,use) -tooltip "Use same connection as existing call" \
        -command {
            global tmp
            $tmp(basicTab).cnc.callMultiplex.01 delete 0 end
            if { $tmp(multiplex,use) } {
                set tmp(multiplex,call) "Select Call"
                foreach call [.test.calls.list get 0 end] {
                    $tmp(basicTab).cnc.callMultiplex.01 add radiobutton \
                        -indicatoron 0 -value $call -variable tmp(multiplex,call) -label $call
                }
            } else { set tmp(multiplex,call) "" }
        }
    menubutton $base.cnc.callMultiplex -height 1 -width 10 -anchor w -indicatoron 1 \
        -menu $base.cnc.callMultiplex.01 -relief raised -text "" -textvariable tmp(multiplex,call) -tooltip "Select connection to use"
    menu $base.cnc.callMultiplex.01 -activeborderwidth 1 -tearoff 0

    # Calls buttons
    frame $base.callBut -borderwidth 2 -relief groove
    frame $base.callBut.bw -height 34 -relief sunken -width 58
    label $base.callBut.bw.lab -borderwidth 0 -text "Bandwidth" -tooltip "Set/Change the call bandwidth"
    menubutton $base.callBut.bw.bwValues \
        -height 1 -indicatoron 1 \
        -menu $base.callBut.bw.bwValues.m -pady 2 \
        -relief raised -text "128000" -textvariable app(test,bw) -width 2
    menu $base.callBut.bw.bwValues.m -activeborderwidth 1 -tearoff 0
    foreach lab { "64000" "128000" "192000" "256000" "320000" "384000" "Multirate" } {
        $base.callBut.bw.bwValues.m add radiobutton -indicatoron 0 -value $lab \
            -variable app(test,bw) -label "$lab" -command {Call.SetRate [selectedItem .test.calls.list] $app(test,bw)}
    }
    button $base.callBut.drop \
         -text "Drop" -width 6 -highlightthickness 1 -borderwidth 2 -tooltip "Terminate the selected call (Ctrl-d)" \
         -command {Call.Drop [selectedItem .test.calls.list] $app(test,callDropReason)
                after idle {call:SetButtonsState $tmp(basicTab)}
         }

    button $base.callBut.dropAll \
        -command {Call.DropAll $app(test,callDropReason)} \
        -text "Drop All" -width 6 -borderwidth 2 -highlightthickness 1 -tooltip "Terminate all the calls (Ctrl-a)"
    frame $base.callBut.reason -relief sunken
    label $base.callBut.reason.lab -borderwidth 0 -text "Drop Reason" -tooltip "Set call drop/reject reason"
    menubutton $base.callBut.reason.menu \
        -height 1 -indicatoron 1 \
        -menu $base.callBut.reason.menu.m -pady 2 -relief raised -text "11" \
        -textvariable app(test,callDropReason)
    menu $base.callBut.reason.menu.m -activeborderwidth 1 -tearoff 0
    foreach {value reason} {
        "NoBandwidth" " 0 - No bandwidth"
        "GatekeeperResource" " 1 - Gatekeeper resources"
        "UnreachableDestination" " 2 - Unreachable destination"
        "DestinationRejection" " 3 - Destination rejection"
        "InvalidRevision" " 4 - Invalid revision"
        "NoPermision" " 5 - No permission"
        "UnreachableGatekeeper" " 6 - Unreachable gatekeeper"
        "GatewayResource" " 7 - Gateway resources"
        "BadFormatAddress" " 8 - Bad format address"
        "AdaptiveBusy" " 9 - Adaptive busy"
        "InConf" "10 - In conf"
        "UndefinedReason" "11 - Undefined reason"
        "FacilityCallDeflection" "15 - Facility call deflection"
        "SecurityDenied" "16 - Security denied"
        "CalledPartyNotRegistered" "17 - Called party not registered"
        "CallerNotregistered" "18 - Caller not registered"
        "NewConnectionNeeded" "21 - New connection needed"
        "NeededFeatureNotSupported" "26 - Needed feature not supported"
        "HopCountExceeded" "27 - Hop count exceeded" } {
        $base.callBut.reason.menu.m add radiobutton -indicatoron 0 \
            -variable app(test,callDropReason) -label "$reason" -value $value
    }
    button $base.callBut.callProceeding -tooltip "Send CallProceeding Message" \
        -command {Call.SendCallProceeding [selectedItem .test.calls.list]}  \
        -text "Proceeding" -borderwidth 2 -highlightthickness 1 -width 6

    button $base.callBut.alerting -tooltip "Send Alerting Message" \
        -command {Call.SendAlerting [selectedItem .test.calls.list]} \
        -text "Alerting" -borderwidth 2 -highlightthickness 1 -width 6
    button $base.callBut.connect -tooltip "Send Connect message" \
        -command {Call.SendConnect [selectedItem .test.calls.list]} \
        -text "Connect" -borderwidth 2 -highlightthickness 1 -width 6

    # Check buttons
    frame $base.inType -borderwidth 2 -relief groove
    frame $base.multiplex -borderwidth 2 -relief groove
    test:buildCallType $base.inType $base.multiplex

    ########
    # GRID #
    ########

    grid rowconf $base 0 -weight 0 -minsize 7
    grid rowconf $base 1 -weight 0 -minsize 22
    grid rowconf $base 2 -weight 0 -minsize 7
    grid rowconf $base 3 -weight 1
    grid rowconf $base 4 -weight 0 -minsize 7
    grid rowconf $base 5 -weight 1
    grid rowconf $base 6 -weight 0 -minsize 7
    grid rowconf $base 7 -weight 0 -minsize 22
    grid columnconf $base 0 -weight 1

    # Quick Call
    grid $base.quick -in $base -column 0 -row 1 -sticky nesw -padx 2
    grid rowconf $base.quick 0 -weight 0 -minsize 7
    grid columnconf $base.quick 1 -weight 1
    grid $base.quick.make -in $base.quick -column 0 -row 1 -padx 4
    grid $base.quick.histEnt -in $base.quick -column 1 -row 1 -sticky ew -padx 3 -ipady 1 -pady 3

    # conference and connection
    grid $base.cnc -in $base -column 0 -row 3 -sticky nesw -padx 2
    grid rowconf $base.cnc 0 -weight 0 -minsize 2
    grid columnconf $base.cnc 1 -weight 1
    grid $base.cnc.useAdHoc -in $base.cnc  -column 0 -row 1 -sticky w -padx 4
    grid $base.cnc.callAdHoc -in $base.cnc -column 1 -row 1 -sticky ew -padx 2
    grid $base.cnc.useMultiplex -in $base.cnc  -column 0 -row 2 -sticky w -padx 4
    grid $base.cnc.callMultiplex -in $base.cnc -column 1 -row 2 -sticky ew -padx 2

    # Call buttons
    grid $base.callBut -in $base -column 0 -row 5 -sticky nesw -padx 2
    grid rowconf $base.callBut 0 -weight 0 -minsize 5
    grid rowconf $base.callBut 1 -weight 1
    grid rowconf $base.callBut 2 -weight 1
    grid rowconf $base.callBut 3 -weight 1
    grid columnconf $base.callBut 0 -weight 1
    grid columnconf $base.callBut 1 -weight 1
    grid columnconf $base.callBut 2 -weight 1
    grid $base.callBut.bw             -in $base.callBut        -column 0 -row 1 -columnspan 2 -sticky we -padx 6
    grid columnconf $base.callBut.bw 1 -weight 1
    grid $base.callBut.bw.lab           -in $base.callBut.bw     -column 0 -row 0 -padx 2
    grid $base.callBut.bw.bwValues      -in $base.callBut.bw     -column 1 -row 0 -sticky we
    grid $base.callBut.reason         -in $base.callBut        -column 0 -row 2 -columnspan 2  -padx 6 -sticky we
    grid $base.callBut.drop           -in $base.callBut        -column 0 -row 3 -sticky we  -padx 6
    grid $base.callBut.dropAll        -in $base.callBut        -column 1 -row 3 -sticky we  -padx 6
    grid columnconf $base.callBut.reason 1 -weight 1
    grid $base.callBut.reason.lab       -in $base.callBut.reason -column 0 -row 0 -padx 2
    grid $base.callBut.reason.menu      -in $base.callBut.reason -column 1 -row 0 -sticky we
    grid $base.callBut.callProceeding -in $base.callBut        -column 2 -row 1 -sticky we -padx 6
    grid $base.callBut.alerting       -in $base.callBut        -column 2 -row 2 -sticky we -padx 6
    grid $base.callBut.connect        -in $base.callBut        -column 2 -row 3 -sticky we -padx 6

    # Check buttons
    grid $base.inType -in $base -column 1 -row 1 -rowspan 7 -sticky nesw -padx 2 -ipady 0
    grid $base.multiplex -in $base -column 0 -row 7 -sticky nesw -padx 2

    ########
    # BIND #
    ########

    bind $base.quick.histEnt.name <Return> "$base.quick.make invoke"
    bind .test <Control-Key-d> "$base.callBut.drop invoke"
    bind .test <Control-Key-a> "$base.callBut.dropAll invoke"

    placeHeader $base.quick "Quick Call"
    placeHeader $base.cnc "Conference & Connection"
    placeHeader $base.callBut "Call Handling"
    placeHeader $base.inType "Call Type"
    placeHeader $base.multiplex "Multiplex"

    bind .test.calls.list <<ListboxSelect>> "+
            call:SetButtonsState $base"

    call:SetButtonsState $base
}

proc test:buildCallType {typeBase muxBase} {
    checkbutton $typeBase.isFastStart   -text "Fast Start"   -variable app(newcall,fastStart)    -tooltip "Accept FastStart offer"
    checkbutton $typeBase.isTunneling   -text "Tunneling"    -variable app(newcall,tunneling)    -tooltip "Accept Tunneling offer"
    checkbutton $typeBase.parallel      -text "Parallel"     -variable app(newcall,parallel)     -tooltip "Allow both FastStart and Tunneling"
    checkbutton $typeBase.manRas        -text "manual RAS"   -variable app(newcall,manualRAS)    -tooltip "Handle new calls as manual RAS calls"
    checkbutton $typeBase.isOverlapping -text "Overlap Send" -variable app(newcall,canOvsp)      -tooltip "Use overlap receiving procedures"
    checkbutton $typeBase.sendComplete  -text "Complete"     -variable app(newcall,sendComplete) -tooltip "Signal that complete address was sent"
    checkbutton $typeBase.autoAns       -text "Auto Answer"  -variable app(newcall,autoAns)      -tooltip "Answer incoming call automatically"

    menubutton $typeBase.efc -height 1 -indicatoron 1 -width 3 -menu $typeBase.efc.m -pady 3 \
        -relief raised -text "0" -textvariable app(newcall,efc) -tooltip "Use the Extened Fast Connect procedures"
    menu $typeBase.efc.m -activeborderwidth 1 -tearoff 0
    foreach efcType { "No EFC" "Want EFC" "Need EFC" } value {0 1 2} {
        $typeBase.efc.m add radiobutton -indicatoron 0 \
            -variable app(newcall,efc) -label "$efcType" -value $value
    }
    button $typeBase.efcSet -text "Set" -width 3 -tooltip "Set EFC for selected call" \
        -command {EFC.Set [selectedItem .test.calls.list] $app(newcall,efc)}

    menubutton $typeBase.stage -height 1 -indicatoron 1 -width 8 \
        -menu $typeBase.stage.m -pady 2 -relief raised -text "setup" \
        -textvariable app(h245,stage) -tooltip "Select stage to pass H245 address in"
    menu $typeBase.stage.m -activeborderwidth 1 -tearoff 0
    foreach stage { "setup" "call proceeding" "alerting" "connect" "early" "facility" "no H245" } {
        $typeBase.stage.m add radiobutton -indicatoron 0 \
            -variable app(h245,stage) -label $stage -value [string range $stage 0 8] -command { test:SetH245Status $app(h245,stage) }
    }

    grid columnconf $typeBase 0 -weight 1
    grid rowconf $typeBase 0 -minsize 5
    foreach i {6 9} {grid rowconf $typeBase $i -weight 1}
    grid $typeBase.isFastStart   -in $typeBase -column 0 -row 1 -padx 2 -sticky sw
    grid $typeBase.isTunneling   -in $typeBase -column 0 -row 2 -padx 2 -sticky sw
    grid $typeBase.parallel      -in $typeBase -column 0 -row 3 -padx 2 -sticky sw
    grid $typeBase.efc           -in $typeBase -column 0 -row 4 -padx 9 -sticky sw
    grid $typeBase.efcSet        -in $typeBase -column 0 -row 4 -padx 9 -sticky nse
    grid $typeBase.stage         -in $typeBase -column 0 -row 5 -padx 9 -pady 2 -sticky ew
    grid $typeBase.manRas        -in $typeBase -column 0 -row 6 -padx 2 -sticky sw
    grid $typeBase.isOverlapping -in $typeBase -column 0 -row 7 -padx 2 -sticky sw
    grid $typeBase.sendComplete  -in $typeBase -column 0 -row 8 -padx 2 -sticky sw
    grid $typeBase.autoAns       -in $typeBase -column 0 -row 9 -padx 2 -sticky sw

    checkbutton $muxBase.multiplexed   -text "Multiplexed"  -variable app(newcall,multiplexed)  -tooltip "Multiplex connections"
    checkbutton $muxBase.maintain      -text "Maintain"     -variable app(newcall,maintain)     -tooltip "Maintain dropped connections"
    button $muxBase.updt -text "Update" -width 7 -tooltip "Update for selected call" \
        -command {Multiplex.Update [selectedItem .test.calls.list] $app(newcall,multiplexed) $app(newcall,maintain)}

    grid rowconf $muxBase 0 -minsize 5
    grid rowconf $muxBase 1 -weight 1
    grid columnconf $muxBase 0 -weight 1
    grid columnconf $muxBase 1 -weight 1
    grid columnconf $muxBase 2 -weight 1
    grid $muxBase.multiplexed -in $muxBase -column 0 -row 1
    grid $muxBase.maintain    -in $muxBase -column 1 -row 1
    grid $muxBase.updt        -in $muxBase -column 2 -row 1
}

proc test:callTab {base tabButton} {
    global tmp app

    set tmp(callTab) $base

    ###################
    # CREATING WIDGETS
    ###################
    set tmp(newcall,IsMultiRate) 0

    # Action buttons
    frame $base.actions -borderwidth 2 -relief groove
    button $base.actions.okBut -highlightthickness 1 -borderwidth 2 -text Dial -width 6 \
        -tooltip "Dial a call" -command {
			set cmd {Call.Dial}
			lappend cmd $tmp(newcall,address)
			foreach alias [$tmp(callTab).aliasList.txt get 0 end] {
				lappend cmd $alias
			}
			eval $cmd}
    # Dest IP
    frame $base.actions.destIP -borderwidth 0 -relief flat

    # Aliases
    frame $base.destAlias -borderwidth 2 -relief groove
    frame $base.aliasList -borderwidth 2 -relief groove

    # Q931 messages
    frame $base.q931 -borderwidth 2 -relief groove
    foreach {y w l c t} {
        1 status    "Status"    Call.SendStatusInquiry      "Send Status message"
        2 progress  "Progress"  Call.SendProgress           "Send Progress message"
        3 notify    "Notify"    Call.SendNotify             "Send Notify message"
        4 userInf   "User Info" Call.SendUserInformation    "Send UserInformation message"
        5 facility  "Facility"  {Window open .facility}     "Send Facility message"
    } {
        button $base.q931.$w -tooltip $t -text $l -highlightthickness 1 -borderwidth 2 \
            -command "$c \[selectedItem .test.calls.list\] \$app(options,display)"
        grid $base.q931.$w -in $base.q931 -column 0 -row $y -pady 5 -padx 4 -sticky ew
    }

    alias:createEntry $base $base.destAlias $base.aliasList "Destination Aliases" "Destination"
    ip:createEntry "newcall" $base.actions.destIP $base.actions.okBut newcall,address

    # Message supression
    frame $base.suppressions -borderwidth 2 -relief groove
    foreach {y x v m t} {
        0 0 releaseComplete     releaseComplete     "Suppress outgoing releaseComplete messages"
        1 0 endSessionCommand   endSessionCommand   "Suppress outgoing endSessionCommand messages"
        2 0 ras                 "any RAS"           "Suppress any outgoing RAS message"
        0 1 q931                "any Q.931"         "Suppress any outgoing Q.931 message"
        1 1 h245                "any H.245"         "Suppress any outgoing H.245 message"
    } {
        checkbutton $base.suppressions.$v -text $m -variable app(suppress,$v) -tooltip $t
        grid $base.suppressions.$v -in $base.suppressions -column $y -row $x -sticky w -pady 2
    }

    ###########
    # BINDINGS
    ###########
    bind $base.actions.destIP.histEnt.name <Return> "$base.actions.okBut invoke"
    bind $base.destAlias.name <Double-Return> "$base.actions.okBut invoke"

    ###################
    # SETTING GEOMETRY
    ###################
    grid columnconf $base 1 -weight 1 -minsize 100
    grid rowconf $base 0 -weight 0 -minsize 7
    grid rowconf $base 1 -weight 0 -minsize 22
    grid rowconf $base 2 -weight 0 -minsize 7
    grid rowconf $base 3 -weight 0 -minsize 0
    grid rowconf $base 4 -weight 0 -minsize 7
    grid rowconf $base 5 -weight 1 -minsize 0
    grid rowconf $base 6 -weight 0 -minsize 7

    grid $base.actions -in $base -column 0 -columnspan 2 -row 1 -sticky nesw -padx 2
    grid rowconf $base.actions 0 -minsize 7
    grid rowconf $base.actions 1 -weight 1
    grid columnconf $base.actions 1 -weight 1
    grid $base.actions.okBut    -in $base.actions -column 0 -row 1 -padx 4
    grid $base.actions.destIP -in $base.actions -column 1 -row 1 -sticky news -ipady 2 -padx 2 -pady 1

    grid $base.destAlias -in $base -column 0 -row 3 -columnspan 2 -sticky news -ipady 1 -padx 2
    grid $base.aliasList -in $base -column 1 -row 5 -sticky nesw -padx 2

    # Message suppression
    grid $base.suppressions -in $base -column 1 -row 7 -columnspan 2 -padx 2 -sticky snew
    grid columnconf $base.suppressions 0 -weight 1
    grid columnconf $base.suppressions 1 -weight 1
    grid rowconf $base.suppressions 0 -minsize 5

    # Check buttons
    grid $base.q931 -in $base -column 2 -row 1 -rowspan 5 -sticky nesw -padx 2
    grid rowconf $base.q931 0 -weight 0 -minsize 7
    grid rowconf $base.q931 6 -weight 1

    ###########
    # BALLOONS
    ###########
    balloon:set $base.destAlias.menBox "Set alias type"
    balloon:set $base.actions.destIP.histEnt.name "Address to call (Return to Dial)"

    ########
    # OTHER
    ########
    placeHeader $base.actions "Destination IP"
    placeHeader $base.destAlias "Destination Alias"
    placeHeader $base.q931 "Q.931"
    placeHeader $base.suppressions "Suppressions"
}

proc test:chanTab {base tabButton} {
    global tmp app

    set tmp(chanTab) $base

    # Channels buttons
    frame $base.chanBut -borderwidth 2 -relief groove
    button $base.chanBut.new -tooltip "Create a new channel" \
        -text "Open New" -width 11 -padx 1 -pady 1 \
        -command {Channel.ConnectOutgoing [selectedItem .test.calls.list] $tmp(newchan,dataType) "" }
    button $base.chanBut.same -tooltip "Create a channel in the same session as selected incoming one" \
        -text "Same Session" -width 11 -padx 1 -pady 1 \
        -command {Channel.ConnectOutgoing [selectedItem .test.calls.list] $tmp(newchan,dataType) "SameAs" [selectedItem .test.chan.inList] }
    button $base.chanBut.replace -tooltip "Replace selected outgoing channel" \
        -text "Replace" -width 11 -padx 1 -pady 1 \
        -command {Channel.ConnectOutgoing [selectedItem .test.calls.list] $tmp(newchan,dataType) "Replace" [selectedItem .test.chan.outList] }
    button $base.chanBut.answer -tooltip "Answer selected incoming channel" \
        -text "Answer" -width 11 -padx 1 -pady 1 \
        -command {Channel.Answer [selectedItem .test.chan.inList]}
    button $base.chanBut.drop -tooltip "Terminate selected incoming and outgoing channel" \
        -text "Drop" -width 3 -padx 3 -pady 1 \
        -command {Channel.Drop [selectedItem .test.chan.inList] [selectedItem .test.chan.outList] $tmp(test,chanDropReason)}
    button $base.chanBut.dropIn -tooltip "Terminate selected incoming channel" \
        -text "In" -width 2 -padx 2 -pady 1\
        -command {Channel.Drop [selectedItem .test.chan.inList] "" $tmp(test,chanDropReason)}
    button $base.chanBut.dropOut -tooltip "Terminate selected outgoing channel" \
        -text "Out" -width 2 -padx 2 -pady 1 \
        -command {Channel.Drop "" [selectedItem .test.chan.outList] $tmp(test,chanDropReason)}
    frame $base.chanBut.reason -relief sunken
    label $base.chanBut.reason.lab -borderwidth 0 -text "Drop Reason" -tooltip "Set channel drop/reject reason"
    menubutton $base.chanBut.reason.menu \
        -height 1 -indicatoron 1 -width 3 \
        -menu $base.chanBut.reason.menu.m -pady 2 -relief raised -text "0" \
        -textvariable tmp(test,chanDropReason)
    menu $base.chanBut.reason.menu.m -activeborderwidth 1 -tearoff 0
    foreach reason { "0 - UNKNOWN" "1 - REOPEN" "2 - RESERVATION FAILURE" "3 - NORMAL" } value {0 1 2 3} {
        $base.chanBut.reason.menu.m add radiobutton -indicatoron 0 \
            -variable tmp(test,chanDropReason) -label "$reason" -value $value
    }
    button $base.chanBut.loop -tooltip "Activate media loop for selected channel"\
        -text "Media Loop" -width 11 -padx 1 -pady 1 \
        -command {Channel.MediaLoop [selectedItem .test.chan.outList]}
    button $base.chanBut.setRate \
        -text "Rate" -width 4 -padx 1 -pady 1 \
        -command {Channel.Rate $app(chan,rate) [selectedItem .test.chan.outList] [selectedItem .test.chan.inList]}
    entry $base.chanBut.rate -width 6 -textvariable app(chan,rate) \
            -validate key -invcmd bell -vcmd { string is digit %P }
    # Channel Type
    frame $base.chanBut.chanprm -relief sunken
    menubutton $base.chanBut.chanprm.dataTypemen -tooltip "Select data type" \
        -height 1 -indicatoron 1 -width 7 \
        -menu $base.chanBut.chanprm.dataTypemen.01 -pady 2 -relief raised \
        -textvariable tmp(newchan,dataType)
    menu $base.chanBut.chanprm.dataTypemen.01 -activeborderwidth 1 -tearoff 0

    # Extended Fast Connect
    button $base.chanBut.efc -tooltip "Open Extended Fast Connect panel"\
        -text "EFC" -width 11 -padx 1 -pady 1 \
        -command {
            set callString [selectedItem .test.calls.list]
            if {$callString == ""} {return}
            set callHandle [string range $callString [expr [string first " " $callString] + 1] end]
            set efcState [api:cm:CallGetParam $callHandle ExtendedFastConnectState 0]

            set tmp(faststart,audioPort) 2536
            set tmp(faststart,videoPort) 2536
            set tmp(faststart,dataPort) 2536

            if {$efcState == "Approved"} {
                set tmp(facility,type) "undefined"
                eval "Call.OpenOutgoingFaststart {$callString} {}"
            }
        }

    # Channels checkboxes
    frame $base.chanBut.chekBut -borderwidth 0
    checkbutton $base.chanBut.chekBut.autoAccept -tooltip "Automatically accept every incoming channel" \
        -text "Auto accept channels" -variable app(options,autoAccept)
    checkbutton $base.chanBut.chekBut.autoDrop -tooltip "Automatically drop every channel on request from remote side" \
        -text "Auto drop channels" -variable app(options,autoDrop)
    checkbutton $base.chanBut.chekBut.autoAcceptFs -tooltip "Automatically accept every faststart channel proposal" \
        -text "Auto faststart" -variable app(options,autoAcceptFs)
    checkbutton $base.chanBut.chekBut.autoMimic -tooltip "Automatically open the same channel from the local side on bidirectional channels" \
        -text "Auto mimic channels" -variable app(options,autoMimic)
    checkbutton $base.chanBut.chekBut.replay -tooltip "Replay the incoming media on the outgoing channel" \
        -text "Replay Media" -variable app(options,replayMedia)
    checkbutton $base.chanBut.chekBut.conflict -tooltip "Ignore Master-Slave Conflict for incoming channels" \
        -text "Ignore Conflict" -variable app(options,ignoreConf)

    # Manual: H245 buttons
    frame $base.h245 -borderwidth 2 -relief groove
    entry $base.h245.address -width 10 -textvariable tmp(h245,address) -tooltip "Remote H245 address to connect to (fill for Create / filled by listen)"
    set tmp(h245,address) "0.0.0.0:0"
    entry $base.h245.userInput -tooltip "Press <Enter> to send User Input on selected call" \
        -width 6 -textvariable tmp(h245,ui)

    foreach {x y w l c t} {
        0 2 create      "Create"    CreateH245      "Create an H245 session for a call (fill address)"
        0 1 connect     "Connect"   ConnectControl  "Connect H245 control session for selected call"
        1 1 separate    "Separate"  SeperateControl "Separate H245 control session for selected call"
        2 1 close       "Close"     CloseControl    "Close H245 control session for selected call"
        5 1 caps        "TCS"       SendCaps        "Send capability message on selected call"
        6 1 emptyCaps   "TCS0"      SendEmptyCaps   "Send empty capability message on selected call"
        5 2 capsAck     "TCSAck"    SendCapsAck     "Send capability acknowledgement on selected call"
        6 2 ms          "M / S"     SendMSD         "Initiate Master/Slave transaction"
        4 2 loopOff     "Loop Off"  LoopOff         "Cancel media loop for all channels in selected call"
        3 1 rtd         "RTD"       SendRoundTrip   "Send an H245 round trip delay message on selected call"
        4 1 vfu         "VFU"       SendRoundTrip   "Send an H245 video fast update message on selected channel"
    } {
        button $base.h245.$w -tooltip $t -highlightthickness 1 -text $l -width 7 -padx 1 -pady 1 \
            -command "Call.$c \[selectedItem .test.calls.list\] \$tmp(h245,address)"
        grid $base.h245.$w -in $base.h245 -column $x -row $y -padx 6 -pady 4
    }
    $base.h245.vfu config -command {Channel.FastUpdate [selectedItem .test.chan.inList]}

    ########
    # GRID #
    ########

    grid rowconf $base 0 -weight 0 -minsize 7
    grid rowconf $base 1 -weight 4
    grid rowconf $base 2 -weight 0 -minsize 7
    grid rowconf $base 3 -weight 1
    grid columnconf $base 0 -weight 1

    # Channel buttons
    grid $base.chanBut -in $base -column 0 -row 1 -sticky nesw -padx 2
    grid rowconf $base.chanBut 0 -weight 0 -minsize 5
    grid rowconf $base.chanBut 1 -weight 1
    grid rowconf $base.chanBut 2 -weight 1
    grid rowconf $base.chanBut 3 -weight 1
    grid rowconf $base.chanBut 4 -weight 1
    grid columnconf $base.chanBut 4 -weight 1

    grid $base.chanBut.new         -in $base.chanBut -column 0 -row 1 -padx 4
    grid $base.chanBut.same        -in $base.chanBut -column 1 -row 1 -padx 4
    grid $base.chanBut.replace     -in $base.chanBut -column 2 -row 1 -padx 4
    grid $base.chanBut.answer      -in $base.chanBut -column 0 -row 3 -padx 4
    grid $base.chanBut.loop        -in $base.chanBut -column 1 -row 3 -padx 4
    grid $base.chanBut.setRate     -in $base.chanBut -column 2 -row 3 -padx 4 -sticky w
    grid $base.chanBut.rate        -in $base.chanBut -column 2 -row 3 -padx 4 -sticky e -ipady 2
    grid $base.chanBut.reason      -in $base.chanBut -column 0 -columnspan 2 -row 4 -sticky ew -padx 4
    grid $base.chanBut.dropIn      -in $base.chanBut -column 2 -row 4 -padx 4 -sticky w
    grid $base.chanBut.dropOut     -in $base.chanBut -column 2 -row 4 -padx 4 -sticky e
    grid $base.chanBut.drop        -in $base.chanBut -column 2 -row 4 -padx 4
    grid columnconf $base.chanBut.reason 1 -weight 1
    grid $base.chanBut.reason.lab  -in $base.chanBut.reason -column 0 -row 0 -padx 2
    grid $base.chanBut.reason.menu -in $base.chanBut.reason -column 1 -row 0 -sticky ew
    # Channel Type
    grid $base.chanBut.chanprm -in $base.chanBut -column 0 -row 2 -columnspan 2 -sticky ew -padx 4
    grid columnconf $base.chanBut.chanprm 0 -weight 1
    grid $base.chanBut.chanprm.dataTypemen -in $base.chanBut.chanprm -column 0 -row 0 -sticky ew
    # EFC
    grid $base.chanBut.efc -in $base.chanBut -column 2 -row 2 -sticky ew -padx 4
    # Channels checkboxes
    grid $base.chanBut.chekBut     -in $base.chanBut -column 4 -row 0 -rowspan 5 -sticky nes
    grid rowconf $base.chanBut.chekBut 0 -weight 1
    grid rowconf $base.chanBut.chekBut 1 -weight 1
    grid rowconf $base.chanBut.chekBut 2 -weight 1
    grid rowconf $base.chanBut.chekBut 3 -weight 1
    grid rowconf $base.chanBut.chekBut 4 -weight 1
    grid rowconf $base.chanBut.chekBut 5 -weight 1
    grid columnconf $base.chanBut.chekBut 0 -weight 1
    grid $base.chanBut.chekBut.autoAccept   -in $base.chanBut.chekBut -column 0 -row 0 -sticky w
    grid $base.chanBut.chekBut.autoDrop     -in $base.chanBut.chekBut -column 0 -row 1 -sticky w
    grid $base.chanBut.chekBut.autoAcceptFs -in $base.chanBut.chekBut -column 0 -row 2 -sticky w
    grid $base.chanBut.chekBut.autoMimic    -in $base.chanBut.chekBut -column 0 -row 3 -sticky w
    grid $base.chanBut.chekBut.replay       -in $base.chanBut.chekBut -column 0 -row 4 -sticky w
    grid $base.chanBut.chekBut.conflict     -in $base.chanBut.chekBut -column 0 -row 5 -sticky w

    # Manual: H245 buttons
    grid $base.h245 -in $base -column 0 -row 3 -sticky nesw -padx 2
    grid rowconf $base.h245 0 -weight 0 -minsize 6
    grid $base.h245.address   -in $base.h245 -column 1 -row 2 -padx 6 -pady 4 -sticky ew -columnspan 2 -ipady 3
    grid $base.h245.userInput -in $base.h245 -column 3 -row 2 -padx 6 -pady 4 -ipady 3

    ########
    # BIND #
    ########
    placeHeader $base.chanBut "Channel Handling"
    placeHeader $base.h245 "H.245"
    bind .test.calls.list <<ListboxSelect>> "+
            call:SetChanButtonsState $base"
    bind .test.chan.inList <<ListboxSelect>> "+
            channel:SetButtonsState $base"
    bind .test.chan.outList <<ListboxSelect>> "+
            channel:SetButtonsState $base"
    channel:SetButtonsState $base
    call:SetChanButtonsState $base

    bind $base.h245.userInput <Key-Return> {
        Call.UII [selectedItem .test.calls.list] $tmp(h245,ui)
    }
}


proc test:rasTab {base tabButton} {
    global tmp app

    set tmp(rasTab) $base

    # Manual: RAS buttons
    frame $base.ras -borderwidth 2 -relief groove
    button $base.ras.rrq -tooltip "Send RegistrationRequest/GatekeeperRequest message (ctrl-r)" \
        -command rgrq:register -text "Register" -width 11
    button $base.ras.urq -tooltip "Send UnregistrationRequest (partial if aliases selected) message (Ctrl-u)" \
        -text "Unregister" -width 11 \
        -command rgrq:unregister
    button $base.ras.clear -text Clear -width 11 \
        -tooltip "Clear all (Ctrl-c)" \
        -command "$base.rglist.txt delete 0 end
                  $base.rgsource.name delete 0 end
                  $base.rgGK.histEnt.name delete 0 end
                  $base.rgGK.entAlias delete 0 end"
    button $base.ras.reset -text Reset -width 11 \
            -command rgrq:GetRegInfo -tooltip "Reset registration information by current configuration settings"
    button $base.ras.bw    -text BandWidth -width 11 \
            -command {RAS.SendBRQ [selectedItem .test.calls.list] $app(ras,bw)} -tooltip "Send Call Rate Request for the selected bandwidth"
    menubutton $base.ras.bwValues -height 1 -indicatoron 1 -menu $base.ras.bwValues.m \
            -pady 2 -relief raised -text "1280" -textvariable app(ras,bw) -width 2 -tooltip "Select Bandwidth"
    menu $base.ras.bwValues.m -activeborderwidth 1 -tearoff 0
    foreach lab { "64000" "128000" "192000" "256000" "320000" "384000" "Multirate" } {
        $base.ras.bwValues.m add radiobutton -indicatoron 0 -value $lab -variable app(ras,bw) -label $lab
    }
    button $base.ras.nsm -text NSM -width 11 \
            -command {nsd:getParam; RAS.SendNSM $tmp(nsd,str)} -tooltip "Send RAS Non Standard Message"
    checkbutton $base.ras.rai -text AOOR \
            -variable tmp(ras,aoor) -command {RAS.SendRAI $tmp(ras,aoor)} -tooltip "Signal Out Of Resources on or off"
    checkbutton $base.ras.nrr -text "NoReReg" \
            -variable app(ras,rereg) -tooltip "If checked, the app will stop the stack from re-registaring on URQ"

    # RRQ/GRQ buttons
    frame $base.rgsource -borderwidth 2 -relief groove
    frame $base.rglist
    frame $base.rgGK -borderwidth 2 -relief groove
    label $base.rgGK.labAlias -text "Gatekeeper Alias" -width 15
    entry $base.rgGK.entAlias -textvar tmp(rgrq,gkAlias) -width 1

    alias:createEntry $base $base.rgsource $base.rglist "Source Type" "Source"
    ip:createEntry rgrq $base.rgGK $base.ras.rrq "rgrq,address"
    foreach name { "DNS:IPv4" "DNS:IPv6" "SRV:IPv4" "SRV:IPv6" } {ip:addType rgrq $base.rgGK $name}
    grid rowconf $base.rgGK 0 -minsize 7

    set tmp(rasWidget) $base

    ########
    # GRID #
    ########

    grid rowconf $base 0 -weight 0 -minsize 7
    grid rowconf $base 1 -weight 0
    grid rowconf $base 2 -weight 0 -minsize 7
    grid rowconf $base 3 -weight 1
    grid rowconf $base 4 -weight 0 -minsize 7
    grid rowconf $base 5 -weight 0
    grid columnconf $base 0 -weight 1

    # Manual: RAS buttons
    grid $base.ras -in $base -column 1 -rowspan 5 -row 1 -sticky nesw -padx 2
    grid rowconf $base.ras 0 -weight 0 -minsize 5
    foreach rowNum { 1 2 3 4 5 6 7 8 9 10 11 12 } rowWeight {1 1 3 1 1 3 1 1 3 1 0 0} {
        grid rowconf $base.ras $rowNum -weight $rowWeight
    }
    grid $base.ras.rrq    -in $base.ras -column 0 -row 1 -padx 4
    grid $base.ras.urq    -in $base.ras -column 0 -row 2 -padx 4
    grid $base.ras.clear  -in $base.ras -column 0 -row 4 -padx 4
    grid $base.ras.reset  -in $base.ras -column 0 -row 5 -padx 4
    grid $base.ras.bw     -in $base.ras -column 0 -row 7 -padx 4 -sticky s
    grid $base.ras.bwValues -in $base.ras -column 0 -row 8 -padx 4 -sticky nwe -ipady 1
    grid $base.ras.nsm    -in $base.ras -column 0 -row 10 -padx 4
    grid $base.ras.rai    -in $base.ras -column 0 -row 11 -padx 4 -sticky w
    grid $base.ras.nrr    -in $base.ras -column 0 -row 12 -padx 4 -sticky w

    # RRQ/GRQ buttons
    grid $base.rgsource   -in $base -column 0 -row 1 -sticky nsew -ipady 1 -padx 2
    grid $base.rglist     -in $base -column 0 -row 3 -sticky nesw -padx 2
    grid $base.rgGK       -in $base -column 0 -row 5 -sticky nsew -ipady 2 -padx 2
    grid $base.rgGK.labAlias -in $base.rgGK -column 0 -row 3 -sticky w
    grid $base.rgGK.entAlias -in $base.rgGK -column 1 -row 3 -ipady 2 -pady 2 -sticky ew -columnspan 3

    ########
    # BIND #
    ########
    bind .test <Control-Key-r> "$base.ras.rrq invoke"
    bind .test <Control-Key-u> "$base.ras.urq invoke"
    bind $base <Control-Key-c> "$base.clearList invoke"
    bind $base <Double-Return> "$base.ras.rrq invoke"

    placeHeader $base.ras "RAS"
    placeHeader $base.rgGK "Gatekeeper Address"
    placeHeader $base.rgsource "Source Address"
}

proc test:miscTab {base tabButton} {
    global app tmp

    set tmp(miscTab) $base

    # Non Standard Data
    frame $base.nsdFrame -borderwidth 2 -relief groove
    checkbutton $base.nsdFrame.use -tooltip "Add a NonStandardData structure to all messages possible" \
        -text "Use non standard data" -variable app(options,nonStandardData)
    set nsd [nsd:create $base.nsdFrame]
    $nsd configure -borderwidth 0

    # Call information
    frame $base.callInfo -borderwidth 2 -relief groove
    label $base.callInfo.dispLab -text "Display"
    entry $base.callInfo.dispInfo -textvariable app(options,display) \
        -validate key -vcmd { expr { [string length %P] < 131 } } -invcmd bell
    label $base.callInfo.uuLab -text "UserUser"
    entry $base.callInfo.uuInfo -textvariable app(options,userUser) \
        -validate key -vcmd { expr { [string length %P] < 131 } } -invcmd bell
    foreach {y n l v t} {
        7 tos "TOS Value" app(options,tos)          "Type of Service value to set for connections"
        8 hop "Hop Count" app(options,hopCount)     "The hop count to set for outgoing calls"
    } {
        label $base.callInfo.${n}lab -text $l
        entry $base.callInfo.$n -textvariable $v -tooltip $t -validate key -invcmd bell \
            -vcmd { string is digit %P }
        grid $base.callInfo.${n}lab -in $base.callInfo -column 0 -row $y -sticky w -padx 3
        grid $base.callInfo.$n -in $base.callInfo -column 1 -row $y -padx 3 -pady 2 -sticky ew -columnspan 2
    }
    label $base.callInfo.fwLab -text "FW/NAT"
    checkbutton $base.callInfo.fwUse -tooltip "Publish public address in all messages" -variable app(options,useNatAddress) -borderwidth 0
    entry $base.callInfo.fwAddr -tooltip "Public address to publish" -textvariable app(options,natAddress)

    # H.460.17/18/19
    frame $base.h460 -borderwidth 2 -relief groove
    foreach {type lab values} {
        17 "17" {0 "NotUsed" 1 "Encapsulate"}
        18 "18" {0 "NotUsed" 1 "Support"}
        19 "19" {0 "NotUsed" 1 "Client" 2 "Server"}
        } {
            label $base.h460.l$type -text $lab
            menubutton $base.h460.m$type -height 1 -indicatoron 1 -menu $base.h460.m$type.m -pady 2 -relief raised -width 3 \
                -text "0" -textvariable app(h460,cfg$type)
            menu $base.h460.m$type.m -activeborderwidth 1 -tearoff 0
            if {$type != 19} {
                foreach {value cfg} $values {
                    $base.h460.m$type.m add radiobutton -indicatoron 0 \
                        -variable app(h460,cfg$type) -label "$cfg" -value $value
                }
            } else {
                foreach {value cfg} $values {
                    $base.h460.m$type.m add radiobutton -indicatoron 0 \
                        -variable app(h460,cfg$type) -label "$cfg" -value $value \
                        -command setMuxMode
                }
            }
    }
    label $base.h460.muxModeLab -text "MuxMode"
    menubutton $base.h460.muxMode -height 1 -indicatoron 1 -menu $base.h460.muxMode.m -pady 2 -relief raised -width 3 \
        -text "0" -textvariable app(h460,muxMode)
    menu $base.h460.muxMode.m -activeborderwidth 1 -tearoff 0
    $base.h460.muxMode.m add radiobutton -indicatoron 0 -variable app(h460,muxMode) -label "none" -value "0" -command setMuxMode
    $base.h460.muxMode.m add radiobutton -indicatoron 0 -variable app(h460,muxMode) -label "multiplex" -value "1" -command setMuxMode
    $base.h460.muxMode.m add radiobutton -indicatoron 0 -variable app(h460,muxMode) -label "demultiplex" -value "2" -command setMuxMode
    $base.h460.muxMode.m add radiobutton -indicatoron 0 -variable app(h460,muxMode) -label "full" -value "3" -command setMuxMode
    setMuxMode


    ########
    # GRID #
    ########

    grid rowconf $base 0 -weight 0 -minsize 7
    grid rowconf $base 1 -weight 1
    grid rowconf $base 2 -minsize 8
    grid columnconf $base 0 -weight 1

    # Non Standard Data
    grid $base.nsdFrame -in $base -column 0 -row 1 -padx 2 -sticky snew
    grid columnconf $base.nsdFrame 0 -weight 1
    grid rowconf $base.nsdFrame 0 -minsize 5
    grid rowconf $base.nsdFrame 3 -minsize 3
    grid rowconf $base.nsdFrame 10 -weight 1
    grid $base.nsdFrame.use -in $base.nsdFrame -column 0 -row 1 -sticky w -padx 2 -pady 2
    grid $nsd -in $base.nsdFrame -column 0 -row 2 -sticky nesw

    # Call information
    grid $base.callInfo -in $base -column 1 -row 1 -padx 2 -sticky snew
    grid columnconf $base.callInfo 0 -weight 1
    grid rowconf $base.callInfo 0 -minsize 5
    grid rowconf $base.callInfo 3 -minsize 7
    grid rowconf $base.callInfo 6 -minsize 7
    grid rowconf $base.callInfo 10 -weight 1
    grid $base.callInfo.dispLab  -in $base.callInfo -column 0 -row 1 -padx 3 -sticky w -columnspan 2
    grid $base.callInfo.dispInfo -in $base.callInfo -column 0 -row 2 -ipady 1 -padx 6 -pady 2 -sticky ew -columnspan 3
    grid $base.callInfo.uuLab    -in $base.callInfo -column 0 -row 4 -padx 3 -sticky w -columnspan 2
    grid $base.callInfo.uuInfo   -in $base.callInfo -column 0 -row 5 -ipady 1 -padx 6 -pady 2 -sticky ew -columnspan 3
    grid $base.callInfo.fwLab -in $base.callInfo -column 0 -row 9 -ipady 2 -padx 3 -sticky w
    grid $base.callInfo.fwUse -in $base.callInfo -column 1 -row 9 -sticky w
    grid $base.callInfo.fwAddr -in $base.callInfo -column 2 -row 9 -padx 3 -pady 2 -sticky ew

    grid $base.h460 -in $base -column 0 -columnspan 2 -row 3 -padx 2 -sticky snew
    foreach c {0 1 2 3 4 5} {
        grid columnconf $base.h460 $c -weight 1
    }
    grid rowconf $base.h460 0 -minsize 5
    foreach {type c} {"17" 0 "18" 2 "19" 4} {
        grid $base.h460.l$type -in $base.h460 -column $c -row 1 -padx 2 -sticky e
        grid $base.h460.m$type -in $base.h460 -column [expr $c+1] -row 1 -padx 2 -pady 2 -sticky w
    }
    grid $base.h460.muxModeLab  -in $base.h460 -column 3 -row 2 -padx 2 -pady 2 -sticky e -columnspan 2
    grid $base.h460.muxMode  -in $base.h460 -column 5 -row 2 -padx 2 -pady 2 -sticky w

    ########
    # BIND #
    ########
    placeHeader $base.nsdFrame "Non-Standard Data"
    placeHeader $base.callInfo "Call Information"
    placeHeader $base.h460 "H.460 Configuration"
}


proc setMuxMode {} {
    global app tmp

    if {$app(h460,cfg19) == 0} {
        $tmp(miscTab).h460.muxMode configure -state disabled
        set app(h460,muxMode) 0
    } else {
        $tmp(miscTab).h460.muxMode configure -state normal
    }

    if {$app(h460,cfg19) == 1} {
        if {$app(h460,muxMode) == 0} {
            set app(h460,muxMode) 1
        }
        if {$app(h460,muxMode) == 2} {
            set app(h460,muxMode) 3
        }
    }
}


proc test:optionTab {base tabButton} {
    global app tmp

    set tmp(optionTab) $base

    frame $base.internal -borderwidth 2 -relief groove
    checkbutton $base.internal.suppress -text "Message suppression" -variable app(options,suppressMessages) \
        -tooltip "Suppress messages from application's log windows"
    checkbutton $base.internal.wrapperLogs -text "Wrapper logging" -variable app(options,scriptLogs) \
        -tooltip "Add log messages when scripts access any wrapper functions"
    checkbutton $base.internal.popUp -text "Enable pop-ups" -variable app(options,popUp) \
        -tooltip "Allow sub window pop-up for info and immediate controls"
    checkbutton $base.internal.catchLog -text "Catch stack logs" -variable app(options,catchStackLog) \
        -command {Log.Update $app(options,catchStackLog)} \
        -tooltip "Catch the stack's logs and display them in a window or to the standard output"
    checkbutton $base.internal.blink -text "Blink on message" -variable app(options,blinkMessage) \
        -tooltip "Window will blink in the taskbar when a message is received"
    button $base.internal.raslab -borderwidth 0 -text "RAS Address" -tooltip "Click here to copy to clipboard" \
        -command {global tmp; clipboard clear; clipboard append $tmp(options,ras)}
    menubutton $base.internal.ras -height 1 -indicatoron 1 \
        -menu $base.internal.ras.01 -relief sunken \
        -text None -textvariable tmp(options,ras) -width 0 -tooltip "List of local RAS addresses"
    menu $base.internal.ras.01 -activeborderwidth 1 -tearoff 0
    button $base.internal.q931lab -borderwidth 0 -text "Q.931 Address" -tooltip "Click here to copy to clipboard" \
        -command {global tmp; clipboard clear; clipboard append $tmp(options,q931)}
    menubutton $base.internal.q931 -height 1 -indicatoron 1 \
        -menu $base.internal.q931.01 -relief sunken \
        -text None -textvariable tmp(options,q931) -width 0 -tooltip "List of local Q931 addresses"
    menu $base.internal.q931.01 -activeborderwidth 1 -tearoff 0
    button $base.internal.annexElab -borderwidth 0 -text "Annex E Address" -tooltip "Click here to copy to clipboard" \
        -command {global tmp; clipboard clear; clipboard append "ETA:"; clipboard append $tmp(options,annexE)}
    menubutton $base.internal.annexE -height 1 -indicatoron 1 \
        -menu $base.internal.annexE.01 -relief sunken \
        -text None -textvariable tmp(options,annexE) -width 0 -tooltip "List of local AnnexE addresses"
    menu $base.internal.annexE.01 -activeborderwidth 1 -tearoff 0
    label $base.internal.dialFromLab -borderwidth 0 -text "Dial From"
    entry $base.internal.dialFrom -textvariable app(options,dialFrom) -width 0 -tooltip "Address to bind when dialing out Q931 calls"
    button $base.internal.crash -text "Crash" -tooltip "Cause this application to crash" -command {Options.Crash}
    button $base.internal.reset -text "Reset Log" -tooltip "Restart the log with the current rvtele.ini" \
        -command {LogFile:Reset}

    frame $base.external -borderwidth 2 -relief groove
    foreach type {log config script} \
            txt { "Log viewer" "Configuration editor" "Script editor" } \
            i {1 2 3} {
        entry $base.external.${type}Ed -textvariable app(editor,$type) -width 0
        button $base.external.${type}But -text "..." \
            -command "options:chooseEditor $type"
        label $base.external.${type}Lab -borderwidth 0 -text $txt
        grid $base.external.${type}Lab -in $base.external -column 0 -row $i -sticky w -padx 4
        grid $base.external.${type}Ed -in $base.external -column 1 -row $i -sticky ew
        grid $base.external.${type}But -in $base.external -column 2 -row $i -pady 1 -padx 4
    }

    ########
    # GRID #
    ########
    grid columnconf $base 0 -weight 1
    grid rowconf $base 0 -minsize 7
    grid rowconf $base 1 -weight 1
    grid rowconf $base 2 -minsize 7
    grid rowconf $base 3 -weight 1

    grid $base.internal -in $base -column 0 -row 1 -padx 2 -sticky nsew
    grid columnconf $base.internal 2 -weight 1
    grid rowconf $base.internal 0 -minsize 7
    grid rowconf $base.internal 1 -weight 1
    grid rowconf $base.internal 2 -weight 1
    grid rowconf $base.internal 3 -weight 1
    grid rowconf $base.internal 4 -weight 1
    grid rowconf $base.internal 5 -weight 1

    grid $base.internal.suppress    -in $base.internal -column 0 -row 1 -padx 6 -pady 1 -sticky w
    grid $base.internal.wrapperLogs -in $base.internal -column 0 -row 2 -padx 6 -pady 1 -sticky w
    grid $base.internal.popUp       -in $base.internal -column 0 -row 3 -padx 6 -pady 1 -sticky w
    grid $base.internal.catchLog    -in $base.internal -column 0 -row 4 -padx 6 -pady 1 -sticky w
    grid $base.internal.blink       -in $base.internal -column 0 -row 5 -padx 6 -pady 1 -sticky w

    grid $base.internal.raslab      -in $base.internal -column 1 -row 1 -sticky w -padx 3
    grid $base.internal.ras         -in $base.internal -column 2 -row 1 -padx 3 -pady 1 -sticky ew -columnspan 2
    grid $base.internal.q931lab     -in $base.internal -column 1 -row 2 -sticky w -padx 3
    grid $base.internal.q931        -in $base.internal -column 2 -row 2 -padx 3 -pady 1 -sticky ew -columnspan 2
    grid $base.internal.annexElab   -in $base.internal -column 1 -row 3 -sticky w -padx 3
    grid $base.internal.annexE      -in $base.internal -column 2 -row 3 -padx 3 -pady 1 -sticky ew -columnspan 2
    grid $base.internal.dialFromLab -in $base.internal -column 1 -row 4 -sticky w -padx 3
    grid $base.internal.dialFrom    -in $base.internal -column 2 -row 4 -padx 3 -pady 1 -sticky ew -columnspan 2

    grid $base.internal.crash       -in $base.internal -column 1 -row 5 -padx 3 -sticky w
    grid $base.internal.reset       -in $base.internal -column 3 -row 5 -padx 3 -sticky e

    grid $base.external -in $base -column 0 -row 3 -padx 2 -sticky nsew
    grid columnconf $base.external 1 -weight 1
    grid rowconf $base.external 0 -minsize 7
    grid rowconf $base.external 1 -weight 1
    grid rowconf $base.external 2 -weight 1
    grid rowconf $base.external 3 -weight 1

    ########
    # BIND #
    ########
    placeHeader $base.internal "Internal"
    placeHeader $base.external "External"
}


proc updateSecButtons {name1 name2 op} {
    global tmp

    if {($tmp(status,gk) == "Registered") || ($tmp(status,gk) == "Discovered")} {
        set state disabled
    } else {
        set state normal
    }
    set base $tmp(secTab)

    foreach w {anxDsup anxDaut anxEsup anxFsup} {
        $base.mode.$w configure -state $state
    }
    foreach w {senderEnt generalEnt passwordEnt} {
        $base.ids.sec.$w configure -state $state
    }
}


proc test:secTab {base tabButton} {
    global app tmp

    set tmp(secTab) $base
    set tmp(secButton) $tabButton

    trace variable tmp(status,gk) w updateSecButtons

    frame $base.mode -borderwidth 2 -relief groove
    checkbutton $base.mode.anxDsup -justify left -text "Use Annex D" -variable app(sec,AnxDsup) -tooltip "Support use of Annex D"
    checkbutton $base.mode.anxDaut -justify left -text "Use Annex D (Auth)" -variable app(sec,AnxDaut) -tooltip "Support use of Annex D (auth. only)"
    checkbutton $base.mode.anxEsup -justify left -text "Use Annex E" -variable app(sec,AnxEsup) -tooltip "Support use of Annex E"
    checkbutton $base.mode.anxFsup -justify left -text "Use Annex F" -variable app(sec,AnxFsup) -tooltip "Support use of Annex F" \
        -command {
            if {$app(sec,AnxFsup)} {
                set app(sec,checkIncoming) 1
                $tmp(secTab).ids.sec.checkIn configure -state disabled
            } else {
                $tmp(secTab).ids.sec.checkIn configure -state normal
            }
        }

    frame $base.media -borderwidth 2 -relief groove
    checkbutton $base.media.dh512sup  -justify left -text "Diffie-Hellman 512"  -variable app(sec,dh512sup)  -tooltip "Support Diffie-Hellman media encryption procedures 512 bit"
    checkbutton $base.media.dh1024sup -justify left -text "Diffie-Hellman 1024" -variable app(sec,dh1024sup) -tooltip "Support Diffie-Hellman media encryption procedures 1024 bit"
    checkbutton $base.media.dh1536sup -justify left -text "Diffie-Hellman 1536" -variable app(sec,dh1536sup) -tooltip "Support Diffie-Hellman media encryption procedures 1536 bit"
    checkbutton $base.media.userPrime -justify left -text "Use pre-made prime"  -variable app(sec,userPrime) -tooltip "Use predetermined primes"

    frame $base.ids -borderwidth 0
    frame $base.ids.sec -borderwidth 2 -relief groove
    frame $base.ids.ld -borderwidth 2 -relief groove
    label $base.ids.sec.senderLab   -borderwidth 0 -text "Sender ID"
    label $base.ids.sec.generalLab  -borderwidth 0 -text "General ID"
    label $base.ids.sec.passwordLab -borderwidth 0 -text "Password"
    label $base.ids.sec.remoteLab   -borderwidth 0 -text "Remote ID"
    label $base.ids.ld.servLab      -borderwidth 0 -text "LDAP Server"
    label $base.ids.ld.dnLab        -borderwidth 0 -text "LDAP DN"
    label $base.ids.ld.passLab      -borderwidth 0 -text "LDAP Pass"
    label $base.ids.ld.groupLab     -borderwidth 0 -text "LDAP Group"

    entry $base.ids.sec.senderEnt -textvariable app(sec,senderID) -width 12 \
            -validate key -invcmd bell -vcmd { expr { [string length %P] < 64 } }
    entry $base.ids.sec.generalEnt -textvariable app(sec,generalID) -width 12 \
            -validate key -invcmd bell -vcmd { expr { [string length %P] < 64 } }
    entry $base.ids.sec.passwordEnt -textvariable app(sec,password) -width 12 \
            -validate key -invcmd bell -vcmd { expr { [string length %P] < 64 } }
    entry $base.ids.sec.remoteEnt -textvariable app(sec,remoteID) -width 12 \
            -validate key -invcmd bell -vcmd { expr { [string length %P] < 64 } }
    entry $base.ids.ld.servEnt -textvariable app(ldap,serverName) -width 12 \
            -validate key -invcmd bell -vcmd { expr { [string length %P] < 64 } }
    entry $base.ids.ld.dnEnt -textvariable app(ldap,userDN) -width 12 \
            -validate key -invcmd bell -vcmd { expr { [string length %P] < 64 } }
    entry $base.ids.ld.passEnt -textvariable app(ldap,password) -width 12 \
            -validate key -invcmd bell -vcmd { expr { [string length %P] < 64 } }
    entry $base.ids.ld.groupEnt -textvariable app(ldap,group) -width 12 \
            -validate key -invcmd bell -vcmd { expr { [string length %P] < 64 } }

    checkbutton $base.ids.sec.checkIn -justify left -text "Check incming messages" -variable app(sec,checkIncoming) \
        -tooltip "Check incoming messages"
    checkbutton $base.ids.ld.useLdap -justify left -text "Use LDAP" -variable app(sec,useLdap) -state disabled \
        -tooltip "Use LDAP server for security"
    button $base.ids.ld.initLdap -text "Initialize" -relief raised -border 1 -command { \
			LDAP.init $app(ldap,serverName) $app(ldap,userDN) $app(ldap,password) $app(ldap,group)} \
        -tooltip "Activates the LDAP module" -padx 1 -pady 1
    button $base.ids.ld.endLdap -text "End" -relief raised -border 1 -command {LDAP.end} \
        -tooltip "Terminate the LDAP module" -padx 1 -pady 1

    ########
    # GRID #
    ########
    grid columnconf $base 0 -weight 1
    grid columnconf $base 1 -weight 3
    grid rowconf $base 0 -minsize 7
    grid rowconf $base 1 -weight 1
    grid rowconf $base 2 -minsize 7
    grid rowconf $base 3 -weight 1

    grid $base.mode -in $base -column 0 -row 1 -padx 2 -sticky news
    grid rowconf $base.mode 0 -minsize 7
    grid rowconf $base.mode 1 -weight 1
    grid rowconf $base.mode 2 -weight 1
    grid rowconf $base.mode 3 -weight 1
    grid rowconf $base.mode 4 -weight 1
    grid columnconf $base.mode 0 -weight 1
    grid $base.mode.anxDsup -in $base.mode -column 0 -row 1 -padx 2 -sticky w
    grid $base.mode.anxDaut -in $base.mode -column 0 -row 2 -padx 2 -sticky w
    grid $base.mode.anxEsup -in $base.mode -column 0 -row 3 -padx 2 -sticky w
    grid $base.mode.anxFsup -in $base.mode -column 0 -row 4 -padx 2 -sticky w

    grid $base.media -in $base -column 0 -row 3 -padx 2 -sticky news
    grid rowconf $base.media 0 -minsize 7
    grid rowconf $base.media 1 -weight 1
    grid rowconf $base.media 2 -weight 1
    grid rowconf $base.media 3 -weight 1
    grid rowconf $base.media 4 -weight 1
    grid columnconf $base.media 0 -weight 1
    grid $base.media.dh512sup  -in $base.media -column 0 -row 1 -padx 2 -sticky w
    grid $base.media.dh1024sup -in $base.media -column 0 -row 2 -padx 2 -sticky w
    grid $base.media.dh1536sup -in $base.media -column 0 -row 3 -padx 2 -sticky w
    grid $base.media.userPrime -in $base.media -column 0 -row 4 -padx 2 -sticky w

    grid $base.ids -in $base -column 1 -row 1 -padx 2 -sticky news -rowspan 3
    grid rowconf $base.ids 0 -weight 1
    grid rowconf $base.ids 1 -minsize 7
    grid rowconf $base.ids 3 -weight 1
    grid columnconf $base.ids 0 -weight 1

    grid $base.ids.sec -in $base.ids -column 0 -row 0 -sticky news
    grid rowconf $base.ids.sec 0 -minsize 7
    foreach row {1 2 3 4 6} {
        grid rowconf $base.ids.sec $row -weight 1
    }
    grid columnconf $base.ids.sec 1 -weight 1
    grid $base.ids.sec.senderLab   -in $base.ids.sec -column 0 -row 1 -padx 2 -sticky w
    grid $base.ids.sec.senderEnt   -in $base.ids.sec -column 1 -row 1 -padx 3 -sticky ew
    grid $base.ids.sec.generalLab  -in $base.ids.sec -column 0 -row 2 -padx 2 -sticky w
    grid $base.ids.sec.generalEnt  -in $base.ids.sec -column 1 -row 2 -padx 3 -sticky ew
    grid $base.ids.sec.passwordLab -in $base.ids.sec -column 0 -row 3 -padx 2 -sticky w
    grid $base.ids.sec.passwordEnt -in $base.ids.sec -column 1 -row 3 -padx 3 -sticky ew
    grid $base.ids.sec.remoteLab   -in $base.ids.sec -column 0 -row 4 -padx 2 -sticky w
    grid $base.ids.sec.remoteEnt   -in $base.ids.sec -column 1 -row 4 -padx 3 -sticky ew
    grid $base.ids.sec.checkIn     -in $base.ids.sec -column 0 -row 6 -padx 3 -sticky w -columnspan 2

    grid $base.ids.ld -in $base.ids -column 0 -row 3 -sticky news
    grid rowconf $base.ids.ld 0 -minsize 7
    foreach row {1 2 3 4 5 6} {
        grid rowconf $base.ids.ld $row -weight 1
    }
    grid columnconf $base.ids.ld 1 -weight 1
    grid $base.ids.ld.servLab      -in $base.ids.ld -column 0 -row 1 -padx 2 -sticky w
    grid $base.ids.ld.servEnt      -in $base.ids.ld -column 1 -row 1 -padx 3 -sticky ew
    grid $base.ids.ld.dnLab        -in $base.ids.ld -column 0 -row 2 -padx 2 -sticky w
    grid $base.ids.ld.dnEnt        -in $base.ids.ld -column 1 -row 2 -padx 3 -sticky ew
    grid $base.ids.ld.passLab      -in $base.ids.ld -column 0 -row 3 -padx 2 -sticky w
    grid $base.ids.ld.passEnt      -in $base.ids.ld -column 1 -row 3 -padx 3 -sticky ew
    grid $base.ids.ld.groupLab     -in $base.ids.ld -column 0 -row 4 -padx 2 -sticky w
    grid $base.ids.ld.groupEnt     -in $base.ids.ld -column 1 -row 4 -padx 3 -sticky ew
    grid $base.ids.ld.useLdap      -in $base.ids.ld -column 0 -row 6 -padx 3 -sticky w -columnspan 2
    grid $base.ids.ld.initLdap     -in $base.ids.ld -column 0 -row 6 -padx 3           -columnspan 2
    grid $base.ids.ld.endLdap      -in $base.ids.ld -column 0 -row 6 -padx 3 -sticky e -columnspan 2

    ########
    # BIND #
    ########
    placeHeader $base.ids "Identification"
    placeHeader $base.mode "Security Mode"
    placeHeader $base.media "Encryption Mode"
    placeHeader $base.ids.ld "LDAP"
    if {$app(sec,AnxFsup)} {
        set app(sec,checkIncoming) 1
        $tmp(secTab).ids.sec.checkIn configure -state disabled
    }
}


# test:createMenu
# Creates the menu for the main window
# input  : none
# output : none
# return : none
proc test:createMenu {} {
    global tmp app

    set menus {tools advanced help}
    if {$tmp(recorder)} {
        set menus [linsert $menus 2 record]
        set m(record) [record:getmenu]
    }

    # Make sure we've got all the menus here
    set m(tools) {
        { "Config"          0 {}          config:open}
        { "Status"          1 Ctrl+S      "Window open .status"}
        { "Log File"        4 {}          logfile:open}
        { "RTP Information" 4 {}          "Window open .rtpInfo"}
        {separator}
        { "Modules..."      0 {}          "disableModule:create"}
        { "Save Settings"   0 {}          "init:SaveOptions 0"}
        { "Start stack"     3 {}          "api:cm:Start"}
        { "Stop stack"      3 {}          "api:cm:Stop"}
        { "Restart"         0 {}          "test.Restart"}
        { "Rebind"          2 {}          "test.Rebind"}
        { "Exit"            0 {}          "test.Quit"}
    }
    set m(advanced) {
        { "Console input"   0 {}          "Window open .console"}
        {separator}
        { "Execute script"  0 {}          script:load}
        { "Stop script"     0 {}          script:stop}
        { "Describe script" 0 {}          script:describe}
        { "Edit script"     0 {}          script:edit}
    }
    set m(help) {
        { "About"           0 {}          "Window open .about"}
    }

    # Create the main menu and all of the sub menus from the array variable m
    menu .test.main -tearoff 0
    foreach submenu $menus {
        .test.main add cascade -label [string toupper $submenu 0 0] \
            -menu .test.main.$submenu -underline 0
        menu .test.main.$submenu -tearoff 0

        foreach item $m($submenu) {
            set txt [lindex $item 0]

            if {$txt == "separator"} {
                # Put a separator
                .test.main.$submenu add separator
            } else {
                # Put a menu item
                set under [lindex $item 1]
                set key [lindex $item 2]
                set cmd [lindex $item 3]
                .test.main.$submenu add command -label $txt -accel $key -command $cmd -underline $under
            }
        }
    }

    test:updateScriptMenu
}


# test:updateScriptMenu
# Update the menu holding dynamically executed scripts
# input  : none
# output : none
# return : none
proc test:updateScriptMenu {} {
    global tmp app

    if {[.test.main.advanced index end] > 5} {
        .test.main.advanced delete 6 end
    }

    if {[array names app "script,recent"] != ""} {
        set i 1

        # Add dynamic script list menu items
        .test.main.advanced add separator
        foreach item $app(script,recent) {
            .test.main.advanced add command -label "$i $item" -underline 0 \
                -command "script:run {$item}"
            incr i
        }
    }
}


# test:refreshMenu
# This procedure makes sure menu items are enabled/disabled when specific variables status change
# input  : name1    - Array name (app)
#          name2    - Element name in array
#          op       - Operation on variable
# output : none
# return : none
proc test:refreshMenu {name1 name2 op} {
    global tmp app

    # Determine the menu item's state
    if {$tmp($name2) == 0} {
        set state "disabled"
    } else {
        set state "normal"
    }

    # Make sure to update the right menu item
    switch $name2 {
        script,running {
            foreach i {3 4 5} {
                .test.main.advanced entryconfigure $i -state $state
            }
            if {$tmp($name2) == 0} {
                .test.status.mode configure -text "Normal"
            } else {
                .test.status.mode configure -text "Script"
            }
        }
    }
}


# test:updateTimer
# Update the application's timer
# input  : none
# output : none
# return : none
proc test:updateTimer {} {
    global tmp app

    set diff [expr {[clock seconds] - $tmp(testapp,start)} ]

    set d [expr $diff / 86400]
    set h [expr ($diff / 3600) % 24]
    set m [expr ($diff / 60) % 60]

    .test.status.timer configure -text [format "%d - %02d:%02d" $d $h $m]

    # Make sure we redisplay the time after a minute
    after 60000 test:updateTimer
}


# test:SetCalls
# This procedure sets the status bar call information
# input  : cur      - Current number of calls
#          total    - Total number of calls connected so far
# output : none
# return : none
proc test:SetCalls {cur total} {
    global tmp
    set tmp(totalCalls) $total

    .test.status.calls configure -text "Calls: $cur, $total"
    if {$cur == 0} {
        set tmp(status,check) [after 10000 status:PreCheck]
    } elseif {$tmp(status,check) != -1} {
        after cancel $tmp(status,check)
        set tmp(status,check) -1
    }
}


# test:SetLoggerStatus
# This procedure sets the status bar errors information
# input  : warnings     - Number of warnings we had
#          exceptions   - Number of exceptions we had
#          erros        - Number of errors we had
# output : none
# return : none
proc test:SetLoggerStatus {warnings exceptions errors} {
    global tmp
    if {[array get tmp bgcol] == ""} {
        set tmp(bgcol) [.test.status.log.data cget -foreground]
    }
    .test.status.log.data configure -text "$warnings/$exceptions/$errors"
    if {($exceptions > 0) || ($errors > 0)} {
        .test.status.log.data configure -foreground red
    } else {
        .test.status.log.data configure -foreground $tmp(bgcol)
    }
}


# test:SetGatekeeperStatus
# This procedure sets the status bar gatekeeper status information
# input  : status   - String representing the current registration status
# output : none
# return : none
proc test:SetGatekeeperStatus {status} {
    .test.status.gk configure -text $status
}


# test:SetH245Status
# This procedure sets the status bar h245 status information
# input  : status   - String representing the current registration status
# output : none
# return : none
proc test:SetH245Status {status} {
    .test.status.h245 configure -text $status
}


# test:SetStackStatus
# This procedure sets the status bar stack status information
# input  : status   - String representing the current stack status
# output : none
# return : none
proc test:SetStackStatus {status} {
    .test.status.stack configure -text $status
}


# test:toggleManualButtons
# Toggle the manual mode buttons on the main window on and off.
# This procedure is called by the manual mode checkbox on the main window.
# input  : manualButtonsOn  - "1" indicates that the manual buttons should be displayed on the main
#                             window.
# output : none
# return : none
proc test:toggleManualButtons {manualButtonsOn} {

    if {$manualButtonsOn == 1} {
        # Set manual mode for calls on
        grid .test.manual -in .test -column 3 -row 2 -sticky nesw
        #after 50 {
        #    place .test.manualHeader -x [expr { [winfo x .test.manual] + 9 } ] \
        #        -y [expr { [winfo y .test.manual] - 1 } ] -anchor nw -bordermode ignore }
    } else {
        # Remove manual mode
        grid remove .test.manual
        #place forget .test.manualHeader
    }
}


# test:activateTool
# Activate a tool on the tools bar
# input  : toolname - Name of the tool to activate
# output : none
# return : none
proc test:activateTool {toolname} {

    .test.tools.$toolname configure -relief sunken
}


# test:deactivateTool
# Deactivate a tool on the tools bar
# input  : toolname - Name of the tool to dactivate
# output : none
# return : none
proc test:deactivateTool {toolname} {

    .test.tools.$toolname configure -relief flat
}

bind toolbar <Enter> {
    if [string compare [%W cget -relief] "sunken"] {
        if [string compare [%W cget -state] "disabled"] {
            %W configure -relief raised
        }
    }
}

bind toolbar <Leave> {
    if [string compare [%W cget -relief] "sunken"] {%W configure -relief flat}
}


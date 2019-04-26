
proc test:h450Tab {base tabButton} {
    global tmp app

    set tmp(h450Button) $tabButton
    set tmp(h450Tab) $base

    #default reply
    frame $base.reply -borderwidth 2 -relief groove
    foreach {x w l v t} {
        0   conf    "Confirm"   DefaultAck      "Confirm any services requested"
        1   rejc    "Reject"    DefaultNack     "Reject any services requested"
        2   wait    "Ask"       Wait            "Ask the user for the response"
    } {
        radiobutton $base.reply.$w -tooltip $t -text $l -variable app(h450,defAns) -value $v
        grid $base.reply.$w -in $base.reply -row 0 -column $x -sticky nw -padx 16
    }

    # Services
    ################
    frame $base.service -borderwidth 2 -relief groove

    #transfer - 450.2
    frame $base.service.trans -borderwidth 0
    button $base.service.trans.act -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.callTransfer [selectedItem .test.calls.list] $tmp(h450,trnAddr) $app(h450,trnCall)} \
        -tooltip "Transfer a call to a secondary call"
    histEnt:create $base.service.trans $base.service.trans.act h450,trnAddr "Fill address to transfer to..."
    $base.service.trans.histEnt.name configure -width 16
    menubutton $base.service.trans.call -height 1 -padx 2 -pady 2 -width 10 -anchor w -indicatoron 1 \
        -tooltip "...or Choose a secondary call" \
        -menu $base.service.trans.call.01 -relief raised -text "Select Call" -textvariable app(h450,trnCall)
    menu $base.service.trans.call.01 -activeborderwidth 1 -tearoff 0
    bind $base.service.trans.call <Enter> "callSelect:updt $base.service.trans.call.01 app(h450,trnCall) {$base.service.trans.histEnt.name delete 0 end} "

    #forward and reroute - 450.3
    frame $base.service.fwdrrt -borderwidth 0
    radiobutton $base.service.fwdrrt.selectFwd -text "Forward" -variable app(h450,fwdrrt) -value 0 \
        -command { global app
            place forget $tmp(h450Tab).service.fwdrrt.rrt
            place forget $tmp(h450Tab).service.fwdrrt.res
            place forget $tmp(h450Tab).service.fwdrrt.intr
            place $tmp(h450Tab).service.fwdrrt.histEnt -in $tmp(h450Tab).service.fwdrrt -anchor n -relx 0.5 -y 40
            place $tmp(h450Tab).service.fwdrrt.cond -in $tmp(h450Tab).service.fwdrrt -anchor n -relx 0.5 -y 90
            place $tmp(h450Tab).service.fwdrrt.act -in $tmp(h450Tab).service.fwdrrt -anchor c -x 346 -y 147
            place $tmp(h450Tab).service.fwdrrt.dea -in $tmp(h450Tab).service.fwdrrt -anchor c -x 404 -y 147
        }
    radiobutton $base.service.fwdrrt.selectRrt -text "Reroute" -variable app(h450,fwdrrt) -value 1 \
        -command { global app
            place forget $tmp(h450Tab).service.fwdrrt.act
            place forget $tmp(h450Tab).service.fwdrrt.dea
            place forget $tmp(h450Tab).service.fwdrrt.cond
            place forget $tmp(h450Tab).service.fwdrrt.intr
            place $tmp(h450Tab).service.fwdrrt.histEnt -in $tmp(h450Tab).service.fwdrrt -anchor n -relx 0.5 -y 40
            place $tmp(h450Tab).service.fwdrrt.res -in $tmp(h450Tab).service.fwdrrt -anchor n -relx 0.5 -y 90
            place $tmp(h450Tab).service.fwdrrt.rrt -in $tmp(h450Tab).service.fwdrrt -anchor c -x 346 -y 147
        }
    radiobutton $base.service.fwdrrt.selectInt -text "Interrogate" -variable app(h450,fwdrrt) -value 2 \
        -command { global app
            place forget $tmp(h450Tab).service.fwdrrt.act
            place forget $tmp(h450Tab).service.fwdrrt.dea
            place forget $tmp(h450Tab).service.fwdrrt.rrt
            place forget $tmp(h450Tab).service.fwdrrt.res
            place forget $tmp(h450Tab).service.fwdrrt.histEnt
            place $tmp(h450Tab).service.fwdrrt.cond -in $tmp(h450Tab).service.fwdrrt -anchor n -relx 0.5 -y 90
            place $tmp(h450Tab).service.fwdrrt.intr -in $tmp(h450Tab).service.fwdrrt -anchor c -x 346 -y 147
        }
    button $base.service.fwdrrt.act -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.forwardRequest $tmp(h450,fwdRrtAddr) $app(h450,fwdCond) 1 } \
        -tooltip "Activate the Call Forwarding service"
    button $base.service.fwdrrt.dea -text "Deactivate" -padx 2 -pady 2 -width 8 \
        -command { H450.forwardRequest $tmp(h450,fwdRrtAddr) $app(h450,fwdCond) 0 } \
        -tooltip "Cancel the Call Forwarding service"
    button $base.service.fwdrrt.rrt -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.callReroute [selectedItem .test.calls.list] $tmp(h450,fwdRrtAddr) $app(h450,rrtRes) } \
        -tooltip "Reroute an incoming call"
    button $base.service.fwdrrt.intr -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.forwardInterrogate $app(h450,fwdCond) } -tooltip "Quary destination for forward list"
    histEnt:create $base.service.fwdrrt $base.service.fwdrrt.act h450,fwdRrtAddr "Fill the address to which to Forward/Reroute"
    set command "+
            histEnt:add $base.service.fwdrrt h450,fwdRrtAddr"
    bind $base.service.fwdrrt.dea <1> $command
    bind $base.service.fwdrrt.rrt <1> $command
    $base.service.fwdrrt.histEnt.name configure -width 16
    frame $base.service.fwdrrt.tul -borderwidth 0 -relief raised
    menubutton $base.service.fwdrrt.cond -height 1 -padx 2 -pady 2 -width 10 -indicatoron 1 \
        -tooltip "Choose the conditions in which to Forward" \
        -menu $base.service.fwdrrt.cond.01 -relief raised -text "Unconditional" -textvariable app(h450,fwdCond)
    menu $base.service.fwdrrt.cond.01 -activeborderwidth 1 -tearoff 0
    foreach type { Unconditional Busy NoResponse } {
        $base.service.fwdrrt.cond.01 add radiobutton \
            -indicatoron 0 -value $type -variable app(h450,fwdCond) -label $type
    }
    menubutton $base.service.fwdrrt.res -height 1 -padx 2 -pady 2 -width 10 -indicatoron 1 \
        -tooltip "Choose Reroute reason" \
        -menu $base.service.fwdrrt.res.01 -relief raised -text "Unknown" -textvariable app(h450,rrtRes)
    menu $base.service.fwdrrt.res.01 -activeborderwidth 1 -tearoff 0
    foreach type { Unknown Unconditional Busy NoResponse } {
        $base.service.fwdrrt.res.01 add radiobutton \
            -indicatoron 0 -value $type -variable app(h450,rrtRes) -label $type
    }

    #hold - 450.4
    frame $base.service.hold -borderwidth 0
    button $base.service.hold.act -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.callHold [selectedItem .test.calls.list] $app(h450,nearHold) } \
        -tooltip "Puts a call on Hold"
    button $base.service.hold.ret -text "Deactivate" -padx 2 -pady 2 -width 8 \
        -command { H450.callHoldRtrv [selectedItem .test.calls.list] } \
        -tooltip "Retrieves a call from Hold"
    checkbutton $base.service.hold.near -text "Near" -variable app(h450,nearHold) \
        -width 0 -anchor w -tooltip "Chooses between Near Hold and Remote Hold"

    #park pickup - 450.5
    frame $base.service.prkpku -borderwidth 0
    radiobutton $base.service.prkpku.selectPrk -text "Park" -variable app(h450,prkpku) -value 0 \
        -command { global app
            place forget $tmp(h450Tab).service.prkpku.actPku
            place forget $tmp(h450Tab).service.prkpku.calls
            place forget $tmp(h450Tab).service.prkpku.localPk
            place $tmp(h450Tab).service.prkpku.actPrk      -in $tmp(h450Tab).service.prkpku -anchor c -x 346 -y 147
            place $tmp(h450Tab).service.prkpku.enablePrk   -in $tmp(h450Tab).service.prkpku -anchor w -x 310 -y 70
            place $tmp(h450Tab).service.prkpku.alertingPrk -in $tmp(h450Tab).service.prkpku -anchor w -x 310 -y 90
            place $tmp(h450Tab).service.prkpku.insEP       -in $tmp(h450Tab).service.prkpku -anchor n -relx 0.475 -y 65
            place $tmp(h450Tab).service.prkpku.delEP       -in $tmp(h450Tab).service.prkpku -anchor n -relx 0.525 -y 65
            place $tmp(h450Tab).service.prkpku.eps         -in $tmp(h450Tab).service.prkpku -anchor n -relx 0.5 -y 90
            place $tmp(h450Tab).service.prkpku.histEnt     -in $tmp(h450Tab).service.prkpku -anchor n -relx 0.5 -y 40
        }
    radiobutton $base.service.prkpku.selectPku -text "Pickup" -variable app(h450,prkpku) -value 1 \
        -command { global app
            place forget $tmp(h450Tab).service.prkpku.actPrk
            place forget $tmp(h450Tab).service.prkpku.enablePrk
            place forget $tmp(h450Tab).service.prkpku.alertingPrk
            place forget $tmp(h450Tab).service.prkpku.eps
            place forget $tmp(h450Tab).service.prkpku.delEP
            place forget $tmp(h450Tab).service.prkpku.insEP
            place forget $tmp(h450Tab).service.prkpku.histEnt
            place $tmp(h450Tab).service.prkpku.actPku -in $tmp(h450Tab).service.prkpku -anchor c -x 346 -y 147
            place $tmp(h450Tab).service.prkpku.calls -in $tmp(h450Tab).service.prkpku -anchor n -relx 0.5 -y 90
            place $tmp(h450Tab).service.prkpku.localPk -in $tmp(h450Tab).service.prkpku -anchor w -x 310 -y 70
        }
    button $base.service.prkpku.actPrk -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.callPark [selectedItem .test.calls.list] $tmp(h450,parkingLotAddr) } \
        -tooltip "Activate the Park service"
    button $base.service.prkpku.actPku -text "Activate" -padx 2 -pady 2 -width 8 \
        -command {
            set callStr [selectedItem .test.calls.list]
            set puStr [set tmp(h450,callPku)]
            set puStr [string map { "[" "\\[" "]" "\\]" } $puStr]
            set isLocal $app(h450,localPickup)
            eval "H450.callPick $isLocal \"$callStr\" $puStr" } \
        -tooltip "Activate the Pickup service"
    histEnt:create $base.service.prkpku $base.service.prkpku.actPrk h450,parkingLotAddr "Fill the address of the parking lot"
    $base.service.prkpku.histEnt.name configure -width 16
    menubutton $base.service.prkpku.calls -height 1 -padx 2 -pady 2 -width 10 -indicatoron 1 \
        -menu $base.service.prkpku.calls.01 -relief raised -text "Pick Call" -textvariable tmp(h450,callPku) \
        -tooltip "Choose a call to pick up"
    menu $base.service.prkpku.calls.01 -activeborderwidth 1 -tearoff 0
    button $base.service.prkpku.insEP -image sunkDown \
        -command { $tmp(h450Tab).service.prkpku.eps.01 add radiobutton -indicatoron 0 \
                -label $tmp(h450,parkingLotAddr) -command [ gui:selectedActive $tmp(h450Tab).service.prkpku.eps.01 ]
                h4505SetGroup $tmp(h450Tab).service.prkpku.eps.01
            }
    button $base.service.prkpku.delEP -image sunkSlash \
        -command { $tmp(h450Tab).service.prkpku.eps.01 delete active
                $tmp(h450Tab).service.prkpku.eps.01 activate none
                h4505SetGroup $tmp(h450Tab).service.prkpku.eps.01
        }
    menubutton $base.service.prkpku.eps -height 1 -padx 2 -pady 2 -width 10 -indicatoron 1 \
        -menu $base.service.prkpku.eps.01 -relief raised -text "EndPoints" -textvariable tmp(h450,notifyEP) \
        -tooltip "List of notified EndPoints"
    menu $base.service.prkpku.eps.01 -activeborderwidth 1 -tearoff 0
    checkbutton $base.service.prkpku.enablePrk -text "Enable" -variable app(h450,enablePrk) \
        -width 20 -anchor w -tooltip "Enables being parked in"
    checkbutton $base.service.prkpku.alertingPrk -text "Park on Alerting" -variable app(h450,alertingPrk) \
        -width 20 -anchor w -tooltip "Activates the Park service when sending Alerting"
    checkbutton $base.service.prkpku.localPk -text "Local Pickup" -variable app(h450,localPickup) \
        -width 20 -anchor w -tooltip "Pickup from the local (this) endpoint"

    #wait - 450.6
    frame $base.service.wait -borderwidth 0
    button $base.service.wait.act -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.callWait [selectedItem .test.calls.list] } \
        -tooltip "Activate call waiting service for selected call"
    checkbutton $base.service.wait.disable -text "Disable" -variable app(h450,disableWait) \
        -width 20 -anchor w \
        -tooltip "Disables the Call Waiting service for future calls"

    #message waiting - 450.7
    frame $base.service.mwi -borderwidth 0
    radiobutton $base.service.mwi.selectMes -text "Message" -variable app(h450,indication) -value 0 \
        -command { global app
            place forget $tmp(h450Tab).service.mwi.callBack
            place forget $tmp(h450Tab).service.mwi.deactivateCB
            place forget $tmp(h450Tab).service.mwi.interogate
            place $tmp(h450Tab).service.mwi.message -in $tmp(h450Tab).service.mwi -anchor c -x 346 -y 147
            place $tmp(h450Tab).service.mwi.deactivateMes -in $tmp(h450Tab).service.mwi -anchor c -x 404 -y 147
        }
    radiobutton $base.service.mwi.selectCB -text "Callback" -variable app(h450,indication) -value 1 \
        -command { global app
            place forget $tmp(h450Tab).service.mwi.message
            place forget $tmp(h450Tab).service.mwi.deactivateMes
            place forget $tmp(h450Tab).service.mwi.interogate
            place $tmp(h450Tab).service.mwi.callBack -in $tmp(h450Tab).service.mwi -anchor c -x 346 -y 147
            place $tmp(h450Tab).service.mwi.deactivateCB -in $tmp(h450Tab).service.mwi -anchor c -x 404 -y 147
        }
    radiobutton $base.service.mwi.selectIntr -text "Interrogate" -variable app(h450,indication) -value 2 \
        -command { global app
            place forget $tmp(h450Tab).service.mwi.callBack
            place forget $tmp(h450Tab).service.mwi.deactivateCB
            place forget $tmp(h450Tab).service.mwi.message
            place forget $tmp(h450Tab).service.mwi.deactivateMes
            place $tmp(h450Tab).service.mwi.interogate -in $tmp(h450Tab).service.mwi -anchor c -x 346 -y 147
        }
    button $base.service.mwi.message -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.MC.Activate $tmp(h450,MWIsendTo) 1 0} \
        -tooltip "Send a Message Indication to client"
    button $base.service.mwi.callBack -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.MC.Activate $tmp(h450,MWIsendTo) 1 1} \
        -tooltip "Send a Callback Indication to client"
    button $base.service.mwi.deactivateMes -text "Deactivate" -padx 2 -pady 2 -width 8 \
        -command { H450.MC.Activate $tmp(h450,MWIsendTo) 0 0} \
        -tooltip "Send a Message Deactivation to client"
    button $base.service.mwi.deactivateCB -text "Deactivate" -padx 2 -pady 2 -width 8 \
        -command { H450.MC.Activate $tmp(h450,MWIsendTo) 0 1} \
        -tooltip "Send a CallBack Deactivation to client"
    button $base.service.mwi.interogate -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.SU.Interogate $tmp(h450,MWIsendTo) } \
        -tooltip "Send an Interogation message to server"
    histEnt:create $base.service.mwi $base.service.mwi.message h450,MWIsendTo "Fill here the address of the client/server"
    set command "+
            histEnt:add $base.service.mwi h450,MWIsendTo"
    bind $base.service.mwi.message <1> $command
    bind $base.service.mwi.callBack <1> $command
    bind $base.service.mwi.deactivateMes <1> $command
    bind $base.service.mwi.deactivateCB <1> $command
    bind $base.service.mwi.interogate <1> $command
    $base.service.mwi.histEnt.name configure -width 16

    #name ID - 450.8
    frame $base.service.nameID -borderwidth 0
    menubutton $base.service.nameID.type -height 1 -padx 2 -pady 2 -width 10 -indicatoron 1 \
        -menu $base.service.nameID.type.01 -relief raised -text None -textvariable app(h450,nameType) \
        -tooltip "Choose type of name ID"
    menu $base.service.nameID.type.01 -activeborderwidth 1 -tearoff 0
    foreach type {Allowed Restricted NA None} state { normal normal disabled disabled } {
        $base.service.nameID.type.01 add radiobutton \
            -indicatoron 0 -value $type -variable app(h450,nameType) -label $type \
            -command "$base.service.nameID.entName delete 0 end
                    $base.service.nameID.entName config -state $state"
    }
    entry $base.service.nameID.entName -textvariable app(h450,nameID) \
        -tooltip "Fill name ID for Allowed and Restricted types"

    #completion -450.9
    frame $base.service.comp -borderwidth 0
    radiobutton $base.service.comp.a1 -text "User A - Busy" -variable app(h450,completion) -value 1 \
        -command {h4509SetSide 1} -tooltip "Handle User A's actions when it's on Busy"
    radiobutton $base.service.comp.a2 -text "User A - NoResponse" -variable app(h450,completion) -value 0 \
        -command {h4509SetSide 0} -tooltip "Handle User A's actions when it's on NoResponse"
    radiobutton $base.service.comp.b -text "User B" -variable app(h450,completion) -value 2 \
        -command {h4509SetSide 2} -tooltip "Handle User B's actions"
    button $base.service.comp.activate -text "Activate" -padx 2 -pady 2 -width 8 \
        -command {H450.callCompletion $app(h450,completion) $app(h450,canRetainServ) $tmp(h450,compAddr) [selectedItem .test.calls.list]} \
        -tooltip "Send call Setup with completion"
    button $base.service.comp.cancel -text "Cancel" -padx 2 -pady 2 -width 8 \
        -command {H450.callCompletionAction Cancel $tmp(h450,compService)} \
        -tooltip "Cancel selected completion service"
    button $base.service.comp.resume -text "Resume" -padx 2 -pady 2 -width 8 \
        -command {H450.callCompletionAction Resume $tmp(h450,compService)} \
        -tooltip "Indicate User B that User A isn't busy after Suspend"
    button $base.service.comp.ringout -text "Ringout" -padx 2 -pady 2 -width 8 \
        -command {H450.callCompletionAction Ringout $tmp(h450,compService)} \
        -tooltip "Complete the call from User A to User B"
    button $base.service.comp.execPossible -text "ExecPossible" -padx 2 -pady 2 -width 12 \
        -state disable -command {H450.callCompletionAction ExecPossible $tmp(h450,compService)} \
        -tooltip "Indicate User A that User B isn't busy anymore"
    histEnt:create $base.service.comp $base.service.comp.activate h450,compAddr "Fill address to send Setup to"
    $base.service.comp.histEnt.name configure -width 16
    label $base.service.comp.callLab -text "Send with"
    menubutton $base.service.comp.call -height 1 -padx 2 -pady 2 -width 10 -indicatoron 1 \
        -menu $base.service.comp.call.01 -relief raised -text <None> -textvariable tmp(h450,compService) \
        -tooltip "Open service to work with"
    menu $base.service.comp.call.01 -activeborderwidth 1 -tearoff 0
    label $base.service.comp.servLab -text "Receive to"
    menubutton $base.service.comp.serv -height 1 -padx 2 -pady 2 -width 10 -indicatoron 1 \
        -menu $base.service.comp.serv.01 -relief raised -text <None> -textvariable app(h450,compFoundService) \
        -tooltip "Route incoming messages to the selected service"
    menu $base.service.comp.serv.01 -activeborderwidth 1 -tearoff 0

    checkbutton $base.service.comp.canRetainServ -text "Can Retain Service" -variable app(h450,canRetainServ) \
        -width 0 -anchor w -tooltip "Indication if User A can retain the service"
    checkbutton $base.service.comp.retainServ -text "Retain Service" -variable app(h450,retainServ) \
        -width 0 -anchor w -tooltip "User B chooses to retain service or not"
    checkbutton $base.service.comp.retainConn -text "Retain Connection" -variable app(h450,retainConn) \
        -width 0 -anchor w -tooltip "Check to indicate signalling connection retention throughout the service lifetime"
    checkbutton $base.service.comp.busy -text "Busy" -variable app(h450,compBusy) \
        -width 0 -anchor w -tooltip "Indicate that the endpoint is busy or to use BSRequest and not NRRequest"

    #offering - 450.10
    frame $base.service.offer -borderwidth 0
    radiobutton $base.service.offer.selectSend -text "Send" -variable app(h450,offer) -value 0 \
        -command { global app
            place forget $tmp(h450Tab).service.offer.in
            place forget $tmp(h450Tab).service.offer.rua
            place $tmp(h450Tab).service.offer.out -in $tmp(h450Tab).service.offer -anchor n -relx 0.5 -y 90
            place $tmp(h450Tab).service.offer.send -in $tmp(h450Tab).service.offer -anchor c -x 346 -y 147
        }
    radiobutton $base.service.offer.selectRet -text "Accept" -variable app(h450,offer) -value 1 \
        -command { global app
            place forget $tmp(h450Tab).service.offer.send
            place forget $tmp(h450Tab).service.offer.out
            place $tmp(h450Tab).service.offer.rua -in $tmp(h450Tab).service.offer -anchor c -x 346 -y 147
            place $tmp(h450Tab).service.offer.in -in $tmp(h450Tab).service.offer -anchor n -relx 0.5 -y 90
        }
    button $base.service.offer.send -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.callOffer $tmp(h450,offerAddr) $app(h450,overideCFB) } \
        -tooltip "Send call Setup with offering"
    button $base.service.offer.rua -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.remoteUserAlerting [selectedItem .test.calls.list] } -tooltip "Send RUA"
    histEnt:create $base.service.offer $base.service.offer.send h450,offerAddr "Fill address to send Setup to"
    $base.service.offer.histEnt.name configure -width 16
    menubutton $base.service.offer.in -height 1 -padx 2 -pady 2 -width 10 -indicatoron 1 \
        -menu $base.service.offer.in.01 -relief raised -text "Select Call" -textvariable app(h450,offerIn) \
        -tooltip "Incomming calls offered to"
    menu $base.service.offer.in.01 -activeborderwidth 1 -tearoff 0
    menubutton $base.service.offer.out -height 1 -padx 2 -pady 2 -width 10 -indicatoron 1 \
        -menu $base.service.offer.out.01 -relief raised -text "Select Call" -textvariable app(h450,offerOut) \
        -tooltip "Outgoing calls offered"
    menu $base.service.offer.out.01 -activeborderwidth 1 -tearoff 0
    checkbutton $base.service.offer.overide -text "Overide CFB" -variable app(h450,overideCFB) \
        -width 0 -anchor w \
        -tooltip "Check to overide the destination's Call Forwarding on Busy"
    checkbutton $base.service.offer.full -text "Waiting List Full" -variable app(h450,waitListFull) \
        -width 0 -anchor w -tooltip "Check to indicate that waiting list is full"

    #intrusion - 450.11
    frame $base.service.intr -borderwidth 0
    radiobutton $base.service.intr.selectFR -text "Forced Release" -variable app(h450,intrusion) -value 0 \
        -command { global tmp
            place forget $tmp(h450Tab).service.intr.sm
            place forget $tmp(h450Tab).service.intr.ir
            place forget $tmp(h450Tab).service.intr.post
            place forget $tmp(h450Tab).service.intr.callID
            place forget $tmp(h450Tab).service.intr.getCallID
            place $tmp(h450Tab).service.intr.fr -in $tmp(h450Tab).service.intr -anchor c -x 346 -y 147
        }
    radiobutton $base.service.intr.selectIR -text "Intrusion Request" -variable app(h450,intrusion) -value 1 \
        -command { global tmp
            place forget $tmp(h450Tab).service.intr.sm
            place forget $tmp(h450Tab).service.intr.fr
            place forget $tmp(h450Tab).service.intr.callID
            place forget $tmp(h450Tab).service.intr.getCallID
            place $tmp(h450Tab).service.intr.ir -in $tmp(h450Tab).service.intr -anchor c -x 346 -y 147
            place $tmp(h450Tab).service.intr.post -in $tmp(h450Tab).service.intr -anchor n -relx 0.5 -y 90
        }
    radiobutton $base.service.intr.selectSM -text "Silent Monitor" -variable app(h450,intrusion) -value 2 \
        -command { global tmp
            place forget $tmp(h450Tab).service.intr.ir
            place forget $tmp(h450Tab).service.intr.fr
            place forget $tmp(h450Tab).service.intr.post
            place $tmp(h450Tab).service.intr.sm -in $tmp(h450Tab).service.intr -anchor c -x 346 -y 147
            place $tmp(h450Tab).service.intr.callID -in $tmp(h450Tab).service.intr -anchor n -relx 0.55 -y 90
            place $tmp(h450Tab).service.intr.getCallID -in $tmp(h450Tab).service.intr -anchor n -relx 0.45 -y 90
        }
    button $base.service.intr.fr -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.callIntrusion $tmp(h450,intrDestAddr) fr "" $app(h450,intrUseWaiting) [selectedItem .test.calls.list] $app(h450,intrCallID) } \
        -tooltip "Send Forced Release to destination"
    button $base.service.intr.ir -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.callIntrusion $tmp(h450,intrDestAddr) ir $app(h450,intrPostIntr) $app(h450,intrUseWaiting) [selectedItem .test.calls.list] $app(h450,intrCallID) } \
        -tooltip "Send intrusion to destination or post intrusion request on call"
    button $base.service.intr.sm -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.callIntrusion $tmp(h450,intrDestAddr) sm "" $app(h450,intrUseWaiting) [selectedItem .test.calls.list] $app(h450,intrCallID) } \
        -tooltip "Activate silent monitoring for destination's call"
    histEnt:create $base.service.intr $base.service.intr.fr h450,intrDestAddr "Fill here the address of the destination"
    set command "+
            histEnt:add $base.service.intr h450,intrDestAddr"
    bind $base.service.intr.ir <1> $command
    bind $base.service.intr.sm <1> $command
    $base.service.intr.histEnt.name configure -width 16
    menubutton $base.service.intr.post -height 1 -padx 2 -pady 2 -width 10 -indicatoron 1 \
        -menu $base.service.intr.post.01 -relief raised -text "" -textvariable app(h450,intrPostIntr) \
        -tooltip "Choose post intrusion request"
    menu $base.service.intr.post.01 -activeborderwidth 1 -tearoff 0
    foreach type { "" "forced release" "wait on busy" "isolation" } {
        $base.service.intr.post.01 add radiobutton \
            -indicatoron 0 -value $type -variable app(h450,intrPostIntr) -label $type
    }
    entry  $base.service.intr.callID -textvariable app(h450,intrCallID) -width 6 \
        -tooltip "Fill here the call ID of the monitored call"
    button  $base.service.intr.getCallID -text "call ID" -borderwidth 0 \
        -command {set app(h450,intrCallID) [api:cm:CallGetParam [lindex [selectedItem .test.calls.list] 1] CallID]} \
        -tooltip "Click to copy the call ID to clipboad"
    checkbutton $base.service.intr.pos -text "possible" -variable app(h450,intrPossible) \
        -width 0 -anchor w -tooltip "Check to indicate intrusion possible"
    checkbutton $base.service.intr.iso -text "Is Isolated" -variable app(h450,intrIsIsolated) \
        -width 0 -anchor w -tooltip "Check for isolation, uncheck for conference"
    checkbutton $base.service.intr.use -text "Use Waiting" -variable app(h450,intrUseWaiting) \
        -width 0 -anchor w \
        -tooltip "Check to use a call that is in \"wait on busy\" mode, and select the call"
    label $base.service.intr.callHandle -text ""

    #information - 450.12
    frame $base.service.cinf -borderwidth 0
    radiobutton $base.service.cinf.selectReq -text "Request" -variable app(h450,CiAction) -value 0
    radiobutton $base.service.cinf.selectInf -text "Inform" -variable app(h450,CiAction) -value 1
    radiobutton $base.service.cinf.selectBoth -text "All of the above" -variable app(h450,CiAction) -value 2
    button $base.service.cinf.act -text "Activate" -padx 2 -pady 2 -width 8 \
        -command { H450.commonInformation [selectedItem .test.calls.list] $app(h450,CiAction) $tmp(h450,infoDestAddr) } \
        -tooltip "Send Common Information service on selected call"
    histEnt:create $base.service.cinf $base.service.cinf.act h450,infoDestAddr "Fill here the address of the destination or select call"
    $base.service.cinf.histEnt.name configure -width 16

    #selector
    set tmp(h450,base) "$base"
    frame $base.service.action -borderwidth 0
    foreach {v x y w txt tip} {
        2   0   0   trans       "Transfer"      "Transfer a call to another endpoint (H450.2)"
        3   1   0   fwdrrt      "Fwd/Rrt"       "Forward (by server) or Reroute incomming calls to another endpoint (H450.3)"
        4   2   0   hold        "Hold"          "Puts a call on hold (H450.4)"
        5   3   0   prkpku      "Prk/Pku"       "Place and retrieve a call from park terminal (H450.5)"
        6   4   0   wait        "Wait"          "Place an incomming call in a call-waiting mode (H450.6)"
        7   5   0   mwi         "MWI"           "Send a Message Waiting or Call Back Indication to client, and Interogate server (H450.7)"
        8   0   1   nameID      "NameID"        "Send name ID with outgoing Setup, Alerting, Connect and RelComp Busy (H450.8)"
        9   1   1   comp        "Completion"    "Wait on an endpoint until it is free to answer (H450.9)"
       10   2   1   offer       "Offer"         "Have an endpoint callback when it is ready to answer (H450.10)"
       11   3   1   intr        "Intrusion"     "Get in the middle of a call between two other endpoints (H450.11)"
       12   4   1   cinf        "Information"   "Send/ask for information about supported services (H450.12)"
    } {
        radiobutton $base.service.action.$w -text $txt -variable app(h450,service) -value $v -tooltip $tip \
            -command "h450SetService $w"
        grid columnconfig $base.service.action $x -weight 1
        grid $base.service.action.$w -in $base.service.action -row $y -column $x -sticky w
    }

    ########
    # GRID #
    ########

    grid rowconfig $base 0 -minsize 7
    grid rowconfig $base 1 -minsize 24
    grid rowconfig $base 2 -minsize 7
    grid rowconfig $base 3 -weight 1
    grid columnconfig $base 0 -weight 1

    grid $base.reply -in $base -row 1 -column 0 -padx 2 -sticky news

    grid $base.service -in $base -row 3 -column 0 -padx 2 -sticky news
    grid rowconfig $base.service 0 -minsize 4
    grid rowconfig $base.service 2 -weight 1
    grid rowconfig $base.service 3 -minsize 160
    grid columnconfig $base.service 0 -weight 1
    grid $base.service.action -in $base.service -row 1 -column 0 -sticky news

    place $base.service.trans.histEnt -in $base.service.trans -anchor n -relx 0.5 -y 40
    place $base.service.trans.call -in $base.service.trans -anchor n -relx 0.5 -y 90
    place $base.service.trans.act -in $base.service.trans -anchor c -x 346 -y 147

    place $base.service.fwdrrt.selectFwd -in $base.service.fwdrrt -anchor w -x 20 -y 30
    place $base.service.fwdrrt.selectRrt -in $base.service.fwdrrt -anchor w -x 20 -y 50
    place $base.service.fwdrrt.selectInt -in $base.service.fwdrrt -anchor w -x 20 -y 70
    place $base.service.fwdrrt.tul -in $base.service.fwdrrt -height 8 -width 8 -anchor c -x 390 -y 30
    if {$app(h450,fwdrrt) == 0} {
        place $base.service.fwdrrt.cond -in $base.service.fwdrrt -anchor n -relx 0.5 -y 90
        place $base.service.fwdrrt.act -in $base.service.fwdrrt -anchor c -x 346 -y 147
        place $base.service.fwdrrt.dea -in $base.service.fwdrrt -anchor c -x 404 -y 147
        place $base.service.fwdrrt.histEnt -in $base.service.fwdrrt -anchor n -relx 0.5 -y 40
    }
    if {$app(h450,fwdrrt) == 1} {
        place $base.service.fwdrrt.res -in $base.service.fwdrrt -anchor n -relx 0.5 -y 90
        place $base.service.fwdrrt.rrt -in $base.service.fwdrrt -anchor c -x 346 -y 147
        place $base.service.fwdrrt.histEnt -in $base.service.fwdrrt -anchor n -relx 0.5 -y 40
    }
    if {$app(h450,fwdrrt) == 2} {
        place $base.service.fwdrrt.cond -in $base.service.fwdrrt -anchor n -relx 0.5 -y 90
        place $base.service.fwdrrt.intr -in $base.service.fwdrrt -anchor c -x 346 -y 147
    }
    place $base.service.hold.act -in $base.service.hold -anchor c -x 346 -y 147
    place $base.service.hold.ret -in $base.service.hold -anchor c -x 404 -y 147
    place $base.service.hold.near -in $base.service.hold -anchor w -x 310 -y 70

    place $base.service.prkpku.selectPrk -in $base.service.prkpku -anchor w -x 20 -y 30
    place $base.service.prkpku.selectPku -in $base.service.prkpku -anchor w -x 20 -y 50
    if {$app(h450,prkpku) == 0} {
            place $base.service.prkpku.actPrk      -in $base.service.prkpku -anchor c -x 346 -y 147
            place $base.service.prkpku.enablePrk   -in $base.service.prkpku -anchor w -x 310 -y 70
            place $base.service.prkpku.alertingPrk -in $base.service.prkpku -anchor w -x 310 -y 90
            place $base.service.prkpku.insEP       -in $base.service.prkpku -anchor n -relx 0.475 -y 65
            place $base.service.prkpku.delEP       -in $base.service.prkpku -anchor n -relx 0.525 -y 65
            place $base.service.prkpku.eps         -in $base.service.prkpku -anchor n -relx 0.5 -y 90
            place $base.service.prkpku.histEnt     -in $base.service.prkpku -anchor n -relx 0.5 -y 40
    }
    if {$app(h450,prkpku) == 1} {
            place $base.service.prkpku.actPku      -in $base.service.prkpku -anchor c -x 346 -y 147
            place $base.service.prkpku.calls       -in $base.service.prkpku -anchor n -relx 0.5 -y 90
    }

    place $base.service.wait.act -in $base.service.wait -anchor c -x 346 -y 147
    place $base.service.wait.disable -in $base.service.wait -anchor w -x 310 -y 70

    place $base.service.mwi.selectMes -in $base.service.mwi -anchor w -x 20 -y 30
    place $base.service.mwi.selectCB -in $base.service.mwi -anchor w -x 20 -y 50
    place $base.service.mwi.selectIntr -in $base.service.mwi -anchor w -x 20 -y 70
    if {$app(h450,indication) == 0} {
    place $tmp(h450Tab).service.mwi.message -in $tmp(h450Tab).service.mwi -anchor c -x 346 -y 147
    place $tmp(h450Tab).service.mwi.deactivateMes -in $tmp(h450Tab).service.mwi -anchor c -x 404 -y 147
    }
    if {$app(h450,indication) == 1} {
    place $tmp(h450Tab).service.mwi.callBack -in $tmp(h450Tab).service.mwi -anchor c -x 346 -y 147
    place $tmp(h450Tab).service.mwi.deactivateCB -in $tmp(h450Tab).service.mwi -anchor c -x 404 -y 147
    }
    if {$app(h450,indication) == 2} {
    place $tmp(h450Tab).service.mwi.interogate -in $tmp(h450Tab).service.mwi -anchor c -x 346 -y 147
    }
    place $base.service.mwi.histEnt -in $base.service.mwi -anchor n -relx 0.5 -y 40

    place $base.service.nameID.type -in $base.service.nameID -anchor n -relx 0.5 -y 90
    place $base.service.nameID.entName -in $base.service.nameID -anchor n -relx 0.5 -y 40

    place $base.service.comp.a1 -in $base.service.comp -anchor w -x 10 -y 30
    place $base.service.comp.a2 -in $base.service.comp -anchor w -x 10 -y 50
    place $base.service.comp.b -in $base.service.comp -anchor w -x 10 -y 70
    place $base.service.comp.cancel -in $base.service.comp -anchor c -x 404 -y 147
    place $base.service.comp.activate -in $base.service.comp -anchor c -x 346 -y 147
    place $base.service.comp.ringout -in $base.service.comp -anchor c -x 288 -y 147
    place $base.service.comp.resume -in $base.service.comp -anchor c -x 230 -y 147
    place $base.service.comp.execPossible -in $base.service.comp -anchor c -x 50 -y 147

    place $base.service.comp.histEnt -in $base.service.comp -anchor n -relx 0.5 -y 40
    place $base.service.comp.call -in $base.service.comp -anchor n -relx 0.5 -y 70
    place $base.service.comp.callLab -in $base.service.comp -anchor n -relx 0.3 -y 70
    place $base.service.comp.serv -in $base.service.comp -anchor n -relx 0.5 -y 100
    place $base.service.comp.servLab -in $base.service.comp -anchor n -relx 0.3 -y 100
    place $base.service.comp.busy -in $base.service.comp -anchor w -x 310 -y 90

    place $base.service.offer.histEnt -in $base.service.offer -anchor n -relx 0.5 -y 40
    place $base.service.offer.selectSend -in $base.service.offer -anchor w -x 20 -y 30
    place $base.service.offer.selectRet -in $base.service.offer -anchor w -x 20 -y 50
    if {$app(h450,offer)} {
        place $base.service.offer.rua -in $base.service.offer -anchor c -x 346 -y 147
        place $base.service.offer.in -in $base.service.offer -anchor n -relx 0.5 -y 90
    } else {
        place $base.service.offer.send -in $base.service.offer -anchor c -x 346 -y 147
        place $base.service.offer.out -in $base.service.offer -anchor n -relx 0.5 -y 90
    }
    place $base.service.offer.overide -in $base.service.offer -anchor w -x 310 -y 70
    place $base.service.offer.full -in $base.service.offer -anchor w -x 310 -y 90

    place $base.service.intr.selectFR -in $base.service.intr -anchor w -x 20 -y 30
    place $base.service.intr.selectIR -in $base.service.intr -anchor w -x 20 -y 50
    place $base.service.intr.selectSM -in $base.service.intr -anchor w -x 20 -y 70
    if {$app(h450,intrusion) == 0} {
        place $tmp(h450Tab).service.intr.fr -in $tmp(h450Tab).service.intr -anchor c -x 346 -y 147
    }
    if {$app(h450,intrusion) == 1} {
        place $tmp(h450Tab).service.intr.ir -in $tmp(h450Tab).service.intr -anchor c -x 346 -y 147
        place $tmp(h450Tab).service.intr.post -in $tmp(h450Tab).service.intr -anchor n -relx 0.5 -y 90
    }
    if {$app(h450,intrusion) == 2} {
        place $tmp(h450Tab).service.intr.sm -in $tmp(h450Tab).service.intr -anchor c -x 346 -y 147
        place $tmp(h450Tab).service.intr.callID -in $tmp(h450Tab).service.intr -anchor n -relx 0.55 -y 90
        place $tmp(h450Tab).service.intr.getCallID -in $tmp(h450Tab).service.intr -anchor n -relx 0.45 -y 90
    }
    place $base.service.intr.histEnt -in $base.service.intr -anchor n -relx 0.5 -y 40
    place $base.service.intr.pos -in $base.service.intr -anchor w -x 310 -y 70
    place $base.service.intr.iso -in $base.service.intr -anchor w -x 310 -y 90
    place $base.service.intr.use -in $base.service.intr -anchor w -x 310 -y 110
    place $base.service.intr.callHandle -in $base.service.intr -anchor c -x 30 -y 150

    place $base.service.cinf.selectReq -in $base.service.cinf -anchor w -x 20 -y 30
    place $base.service.cinf.selectInf -in $base.service.cinf -anchor w -x 20 -y 50
    place $base.service.cinf.selectBoth -in $base.service.cinf -anchor w -x 20 -y 70
    place $base.service.cinf.act -in $base.service.cinf -anchor c -x 346 -y 147
    place $base.service.cinf.histEnt -in $base.service.cinf -anchor n -relx 0.5 -y 40

    if { $app(h450,action) != "" } {
        grid $app(h450,action) -in $base.service -row 3 -column 0 -sticky news }

    ########
    # BIND #
    ########
    placeHeader $base.reply "Reply"
    placeHeader $base.service "Supplementary Services"

    $base.service.comp.serv.01 add radiobutton -label <None> -value <None> -variable app(h450,compFoundService)
    h4505SetGroup $tmp(h450Tab).service.prkpku.eps.01
    h4509SetSide $app(h450,completion)
}

proc h450SetService {w} {
    global tmp app
    if {$app(h450,action) != ""} {grid remove $app(h450,action)}
    set app(h450,action) "$tmp(h450,base).service.$w"
    grid $app(h450,action) -in $tmp(h450,base).service -row 3 -column 0 -sticky news
}

proc h4505DelGroupInd {idx} {
    global app tmp
    set callMenu $tmp(h450Tab).service.prkpku.calls.01

    if { [$callMenu index end] == "none" } return
    $callMenu activate "none"
    set tmp(h450,callPku) "Pick Call"

    incr idx -1
    $callMenu delete $idx
    if { [$callMenu index end] == "none" } return

    for {set i $idx} {$i <= [$callMenu index end]} {incr i} {
        $callMenu entryconfigure $i -value [expr {$i + 1}]
    }
}

proc h4505SetGroup {w} {
    global app

    set res ""
    set last [$w index end]

    if {$last != "none"} {
        for {set i 0} {$i <= $last} {incr i} {
            set res "$res,[$w entrycget $i -label]"
        }
        set res [string range $res 1 end]
    }

    set app(h450,groupList) $res
}

proc h4509SetSide {side} {
    global app tmp
    set base "$tmp(h450Tab).service.comp"

    # 0 - User A Busy
    # 1 - User A NoResponse
    # 2 - User B
    switch $side {
        0 {
            $base.activate config -state normal
            $base.resume config -state normal
            $base.ringout config -state normal
            $base.execPossible config -state disable
            place $base.canRetainServ -in $base -anchor w -x 310 -y 70
            place forget $base.retainServ
            place forget $base.retainConn
        }
        1 {
            $base.activate config -state normal
            $base.resume config -state normal
            $base.ringout config -state normal
            $base.execPossible config -state disable
            place $base.canRetainServ -in $base -anchor w -x 310 -y 70
            place forget $base.retainServ
            place forget $base.retainConn
        }
        2 {
            $base.activate config -state disable
            $base.resume config -state disable
            $base.ringout config -state disable
            $base.execPossible config -state normal
            place forget $base.canRetainServ
            place $base.retainServ -in $base -anchor w -x 310 -y 70
            place $base.retainConn -in $base -anchor w -x 310 -y 50
        }
    }
}

proc h4509AddService {service} {
    global tmp app
    set w $tmp(h450Tab).service.comp
    $w.call.01 add radiobutton -label $service -value [lindex $service 0] -variable tmp(h450,compService)
    $w.serv.01 add radiobutton -label $service -value [lindex $service 0] -variable app(h450,compFoundService)

    # Always set the new service as the selected one
    set tmp(h450,compService) [lindex $service 0]
    set app(h450,compFoundService) [lindex $service 0]
}

proc h4509RemoveService {service} {
    global tmp app
    set wFrame $tmp(h450Tab).service.comp
    set w $wFrame.call.01

    for {set i 0} {1} {incr i} {
        set val [$w entrycget $i -value]
        if {$val == $service} {
            $w delete $i
            $wFrame.serv.01 delete [expr $i+1]
            break
        } elseif {$val == ""} {break}
    }

    if {$app(h450,compFoundService) == $service} {
        $w activate end
        set app(h450,compFoundService) [$wFrame.serv.01 entrycget end -value]
    }
    if {$tmp(h450,compService) == $service} {
        $wFrame.serv.01 activate end
        set tmp(h450,compService) [$wFrame.serv.01 entrycget end -value]
    }
}

proc callSelect:updt { menuWidget varName selectCommand } {
    global app tmp

    eval "set curSel $$varName"

    $menuWidget delete 0 end

    foreach call [.test.calls.list get 0 end] {
        $menuWidget add radiobutton -indicatoron 0 -value $call -variable $varName -label $call \
            -command $selectCommand
    }

    set index [lsearch -exact [.test.calls.list get 0 end] $curSel]

    if { $index != -1 } {
        set $varName $curSel
    } else {
        set $varName "Select Call"
    }
}


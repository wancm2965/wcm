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
#                                 TAP_ras.tcl
#
#   RAS handling.
#   This file holds all the GUI procedures for ep-gk communication: RRQ, GRQ, URQ, etc.
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
#   REGISTRATION operations
#
##############################################################################################


# rgrq:GetRegInfo
# Get the registration information from the configuration and
# put it inside the RAS tab of the test application
# input  : none
# output : none
# return : none
proc rgrq:GetRegInfo {} {
    global tmp app

    set base $tmp(rasWidget)

    # Clear before we begin
    $base.rglist.txt delete 0 end

    # Fetch some node ids
    set valTree [api:cm:GetValTree]
    set rasConfNode [api:cm:GetRASConfigurationHandle]

    catch {
        set aliasesNode [api:pvt:GetByPath $valTree $rasConfNode "registrationInfo.terminalAlias"]

        # Get the aliases while we can
        set index 1
        set aliasNode [api:pvt:GetByPath $valTree $aliasesNode $index]
        while {$aliasNode >= 0} {
            $base.rglist.txt insert end [api:cm:Vt2Alias $valTree $aliasNode]

            # Get the next one
            set aliasNode -1
            catch {
                incr index
                set aliasNode [api:pvt:GetByPath $valTree $aliasesNode $index]
            }
        }
    }

    catch {
        set gkNode [api:pvt:GetByPath $valTree $rasConfNode "registrationInfo.gatekeeperIdentifier"]
        set tmp(rgrq,gkAlias) [api:pvt:GetString $valTree $gkNode]
    }

    set tmp(rgrq,address) ""
    catch {
        set gkNode [api:pvt:GetByPath $valTree $rasConfNode "manualDiscovery.defaultGatekeeper"]
        set tmp(rgrq,address) [api:app:Vt2Address $valTree $gkNode]
        set tmp(rgrq,addressType) "IP"
    }
    if {$tmp(rgrq,address) == ""} {
        catch {
            set gkNode [api:pvt:GetByPath $valTree $rasConfNode "manualDiscovery.defaultGatekeeperName.gkName"]
            set portValue 0
            catch {
                set portNode [api:pvt:GetByPath $valTree $rasConfNode "manualDiscovery.defaultGatekeeperName.gkPort"]
                set portValue [api:pvt:Get $valTree $portNode]
            }

            if {$portValue > 0} {
                set tmp(rgrq,address) "[api:pvt:GetString $valTree $gkNode]:$portValue"
            } else {
                set tmp(rgrq,address) "[api:pvt:GetString $valTree $gkNode]"
            }
            foreach {q typ} {
                dnsTypeIpv4 "DNS:IPv4" dnsTypeIpv6 "DNS:IPv6"
                dnsTypeSrvIpv4 "SRV:IPv4" dnsTypeSrvIpv6 "SRV:IPv6"} {
                catch {
                    api:pvt:GetByPath $valTree $rasConfNode "manualDiscovery.defaultGatekeeperName.queryType.$q"
                    set tmp(rgrq,addressType) $typ
                }
            }
        }
    }
}


# rgrq:IsAutoRas
# Checks if the configuration uses automatic RAS or not
# input  : none
# output : none
# return : 1 if automatic RAS is used, 0 otherwise
proc rgrq:IsAutoRas {} {
    # Fetch some node ids
    set valTree [api:cm:GetValTree]
    set rasConfNode [api:cm:GetRASConfigurationHandle]

    if { [catch {api:pvt:GetByPath $valTree $rasConfNode "manualRAS"}] } {
        # No ManualRAS flag - we're automatic...
        return 1
    }

    # ManualRAS used
    return 0
}


# rgrq:register
# Register the endpoint
# This procedure is called when the "Register" button is pressed
# input  : none
# output : none
# return : none
proc rgrq:register {} {
    global tmp app

    set base $tmp(rasWidget)

    # Get the aliases we need
    set hasAliases 0
    set aliases ""
    if {[$base.rglist.txt size] > 0} {
        set hasAliases 1
        foreach alias [$base.rglist.txt get 0 end] {
            set aliases "$aliases,$alias"
        }
    }
    if {$hasAliases != 0} {
        set aliases [string range $aliases 1 end]
    }

    # Register the endpoint
    if {$tmp(rgrq,address) == ""} {
        api:cm:Register $hasAliases $aliases "" $tmp(rgrq,gkAlias)
    } else {
        api:cm:Register $hasAliases $aliases "$tmp(rgrq,addressType):$tmp(rgrq,address)" $tmp(rgrq,gkAlias)
    }
}


# rgrq:unregister
# Unregister the endpoint or just some of it's aliases
# This procedure is called when the "Unregister" button is pressed
# input  : none
# output : none
# return : none
proc rgrq:unregister {} {
    global tmp

    set base $tmp(rasWidget)

    # Get the aliases we need
    set hasAliases 0
    set delAliases ""
    set selInd [$base.rglist.txt curselection]
    if {$selInd != ""} {
        set hasAliases 1

        foreach aliasInd $selInd {
            set alias [$base.rglist.txt get $aliasInd]
            set delAliases "$delAliases,$alias"
        }

        set size [llength $selInd]
        for {set index [incr size -1]} {$index >= 0} {incr index -1} {
            $base.rglist.txt delete [lindex $selInd $index]
        }
    }
    if {$hasAliases != 0} {
        set delAliases [string range $delAliases 1 end]
    }

    api:cm:Unregister $hasAliases $delAliases
}

# rgrq:partialURQ
# Unregister some of the endpoint's aliases
# This procedure is called from the hookSendTo when tmp(partialURQ) is set to 1
# input  : none
# output : none
# return : none
proc rgrq:partialURQ { messageId } {
    global tmp

    set tmp(partialURQ) 0
    set base $tmp(rasWidget)
    set delAliases [$base.rglist.txt curselection]
    set hVal [api:cm:GetValTree]
    set index 1

    foreach aliasInd $delAliases {
        # add a node for each alias
        set alias [$base.rglist.txt get $aliasInd]
        set path unregistrationRequest.endpointAlias.${index}.
        set nodeId [api:pvt:BuildByPath $hVal $messageId $path &index]
        api:cm:Alias2Vt $hVal $alias $nodeId
        incr index
    }

    set size [llength $delAliases]
    for {set index [incr size -1]} {$index >= 0} {incr index -1} {
        $base.rglist.txt delete [lindex $delAliases $index]
    }
}



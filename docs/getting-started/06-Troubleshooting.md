# Troubleshooting

## Introduction

I get a lot of questions about the functioning of TAC.  A tester's board will have negative behaviors.
The first step is, turn on logging from the TAC "Preferences" panel.  The log will not write until you
open a device, perform the operations and then close the device. This is by design. You can view the
log using a text editor. If you see the debug board responding with "ok" to the commands, TAC is functioning. 

But you are still having issues. To see why this is, imagine a group of people on a deserted island, they are
the TAC application.  These people can to turn their lights on and off.  The debug board is that
flashlight.  It turns pin signals on and off.  That is all it can do.  A debug board will turn those signals
on and off, regardless of a connected or missing device.  Just like on the deserted island, the people do not
know if there is a ship out there to see the signals.  You might think the ship could send a signal back, but
that ship doesn't have a flashlight, and the devices can't communicate with the debug board.  The communication
is one way.  If you see "ok" in the log, the debug board is acknowledging that it turned on or off the light.

Some of the power cycling features, i.e booting, require timing to enable the feature.  The latest hardware
designs have been changing this timing dependency.  The QTAC team discourages this behavior, but the
hardware teams are free to implement the power on they way they see fit.  As a consequence, I am unable to
help with boot issues.  You will have to talk to the hardware team and ask them what the booting cycle
timing for the button press and pause is.

Many questions are regarding the power on, power off.  Or USB0 and USB1 switches.  If your device is deriving
power from another source, the debug board isn't going to be able to cycle power.  Some devices can derive
power from connected usb devices.  USB1 is a case where we are signaling VBus.  The hardware designer may not
have made that connection in hardware or in firmware.  There are cases where a device's USB1 will start to
function after a firmware upgrade.

Go through the general steps.  Check the logs.  If you still have an issue, you will have to contact the device's
hardware team.  I don't have the ablilty to help you.

## General

Turn on logging on the Preferences Dialog.  This will produce a lag of the communication transactions between the software
and the debug board.  If you [open a GitHub issue](https://github.com/qualcomm/qcom-test-automation-controller/issues/new), the log will be included as an attachment on the issue.

1. Try another USB cable.  They are known to go bad. 
2. Try another debug board on the same device.  If the new debug board works, your debug board was bad.
3. Try another device with your original debug board.  If the the new device works, the problem is with your device.  Try updating the software.
4. Try another device with another debug board.  If the new setup works, there problem is with the old combination.
5. If you reach this point, then turn on the logging
6. Run your simplified test again and generate a report using the Bug Writer tool, or attach the log to a GitHub issue.

## Endpoint Exhaustion

Windows has a hard limit of 96 USB endpoints.  When enough devices are connected, physical or virtual, the Windows OS is unable to add the USB device. 
There is ZERO that QTAC can do about this OS resource exhaustion.  This is an issue with the Windows OS itself.  You can read about it [here](https://kb.plugable.com/questions/756044).

**Lahaina, Kona, Hurricane**
I've been getting some reports that USB0 and USB1 disconnect is not functioning.  Try these steps before filing a trouble ticket. 

- Replace your cables
- Try another device and debug board combination.  Do they work?  If so, it's not QTAC TAC.
- Try the working debug board on the device.  Does it work? It was the debug board.  If it does not work, it is your device.  
- Try flashing the device with the latest software if available.  We have seen instances where the device's software malfunctioned.

On the debug board, make sure the switches, 1 and 2 are in the "on" position on S8.​​​​​​​

![Debug Board](../resources/debug-board.png)

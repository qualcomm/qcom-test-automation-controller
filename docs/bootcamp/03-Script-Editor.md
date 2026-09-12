# AlpacaScript QuickStart

## Introduction

AlpacaScript allows you to create custom commands for the Quick Settings buttons. To write your custom AlpacaScript,
select 'Open Script Editor...' from the File menu or click on the AlpacaScript icon in the toolbar.

You will find a preview as below:

![AlpacaScript Window](../resources/script-editor.png)

> <mark>If you find the AlpacaScript icons to be grey, please select a valid configuration by following the
> [New Configuration](../getting-started/04-TAC-Config-Editor.html#create-new-configuration) guide.</mark>

The AlpacaScript window opens with a default script. You can update the script inplace to get your desired outcome.

## Adding Comments

You can add a comment by appending a `//` before your comment.

## Defining New AlpacaScript Command

Every command block in the AlpacaScript window, begins with the name of command followed by a parentheses and
preceded by a `def` keyword. Example: `def bootToEDL()`.

The following lines contains the primitive commands and the helper commands that get executed. These lines are
prefixed by a **single tab character**.

## Supported Primitive Commands

The table below discusses various primitive commands that are supported with AlpacaScript. Primitive commands can
be used in AlpacaScript without the need of defining the command further.

⚠️ **You should not have a custom command name same as a primitive command name**.

| Command Name | Command Parameter | Usage Example | Description |
| -- | -- | -- | -- |
| logComment | String | logComment == powerOn start == | `logComment` lets you add a log comment in the TAC execution logs when your command is triggered |
| delay | Integer (in milliseconds) | delay 800 | `delay` allows you to add a custom time (in ms) delay between commands. |
| battery | Integer (0 or 1) | battery 0 | Toggle the battery state. 0 for off and 1 for on. |
| usb0 | Integer (0 or 1) | usb0 0 | Toggle the state of USB 0. 0 for off. |
| usb1 | Integer (0 or 1) | usb1 1 | Toggle the state of USB 1. 0 for off. |
| pedl | Integer (0 or 1) | pedl 0 | Toggle the state of primary EDL. 0 for off. |
| sedl | Integer (0 or 1) | sedl 0 | Toggle the state of secondary EDL. 0 for off. |
| pkey | Integer (0 or 1) | pkey 0 | Toggle the state of the power key. 0 for off. |
| volup | Integer (0 or 1) | volup 0 | Toggle the state of the volume up key. 0 for off. Please note that volup 1 means pressing the volume up key and not releasing until a volup 0 is sent. |
| voldn | Integer (0 or 1) | voldn 0 | Toggle the state of the volume down key. 0 for off. Please note that voldn 1 means pressing the volume down key and not releasing until a voldn 0 is sent. |

## Using Existing Custom Commands

If you have an existing custom command that you wish to use within AlpacaScript, you can call the custom command name in your command body.

Example:

```
def powerOffTheDevice() 
	logComment ====== powerOffTheDevice sequence start ======

	battery 0
	usb0 0
	usb1 0

	logComment ====== powerOffTheDevice sequence finish ======

def powerOff()
	logComment ====== powerOff start ======

	powerOffTheDevice

	pkey 0
	volup 0
	voldn 0

	pedl 0

	logComment ====== powerOff finish ======
```
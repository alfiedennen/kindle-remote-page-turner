# Kindle Jailbreak Guide

This guide covers jailbreaking your Kindle using **WinterBreak**, which works on firmware 5.18.0 and earlier.

## Check Your Firmware Version

On your Kindle: **Settings > Device Info**

If your firmware is higher than 5.18.0, you'll need to downgrade first (not covered here).

## Jailbreak with WinterBreak

### Step 1: Prepare Your Kindle

1. Put your Kindle in **Airplane Mode**
2. Connect Kindle to your PC via USB
3. Your Kindle should appear as a USB drive

### Step 2: Download WinterBreak

Download from: https://kindlemodding.org/jailbreaking/WinterBreak/

### Step 3: Copy Files to Kindle

Extract the WinterBreak zip and copy these to your Kindle's root folder:

- `.active_content_sandbox/` folder
- `apps/` folder
- `mesquito/` folder
- `jb.sh` file
- `patchedUks.sqsh` file

### Step 4: Run the Jailbreak

1. Safely eject your Kindle from your PC
2. On your Kindle, go to the **Store** in the menu
3. The jailbreak will start automatically
4. Wait for it to complete (screen may freeze temporarily)
5. If the screen freezes for more than a minute, hold the power button for 40 seconds to force restart
6. Your Kindle will restart when the jailbreak is complete

### Step 5: Install Hotfix

The hotfix prevents your jailbreak from being removed during updates.

1. Download hotfix from: https://kindlemodding.org/jailbreaking/Post-Jailbreak/JailBreak-Hotfix/
2. Connect Kindle to PC
3. Copy `Update_hotfix_universal.bin` to the Kindle root folder
4. Eject Kindle
5. On Kindle: **Settings > Device Options > Advanced Options > Update Your Kindle**

## Verify Jailbreak Success

After restarting, your Kindle is jailbroken. You can now proceed to install KUAL and KOReader.

## Next Steps

Continue to [Kindle Setup](KINDLE-SETUP.md) to install the required software.

README
===========================================================
I8kfanGUI   0.2                               June 15, 2007
I8kfan.kext 1.0.0d1                            May  9, 2007
===========================================================

Written by PotatoSalad

http://potatosaladx.blogspot.com/search/label/I8kfan

===========================================================
Installation
===========================================================

NOTE: This was written and tested on a Dell Inspiron 5100

NOTE: For Inspiron 5100/1100 and possibly other models that
      do not support SpeedStep technology, make sure you
      have AppleIntelCPUPowerManagement.kext loaded.

1.  Open up a Terminal window and try loading I8kfan.kext:
    
    sudo chown -R root:wheel I8kfan.kext
    sudo kextload -v I8kfan.kext

2.  If it loaded, run "sysctl i8k".  You should see this:
    
    i8k.power_status: 1
    i8k.cpu_temp: 40
    i8k.bios_version: -1
    i8k.fan0_speed: 111180
    i8k.fan1_speed: -1
    i8k.fan0_status: 1
    i8k.fan1_status: -1
    
    Or something like that.
    
    If not, then your system probably isn't supported due
    to my poor coding.

3.  If everything has worked thus far, move the extension
    into the /System/Library/Extensions folder:
    
    sudo mv I8kfan.kext /System/Library/Extensions/
    sudo chown -R root:wheel /System/Library/Extensions/I8kfan.kext
    sudo chmod -R 755 /System/Library/Extensions/I8kfan.kext
    
    You may also want to repair permissions using Disk Utility.

4.  You should be able to use the I8kfanGUI application now
    to control the CPU temperature.

===========================================================
Background
===========================================================

My Dell Inspiron 5100 is notorious for overheating.  It has
a Pentium 4 (not mobile) and 1 fan that doesn't kick in when
it should.

On Windows, there is a program called I8kfanGUI that I use
to force the fan to kick in at certain times when the CPU
temperature reaches a certain level.

On Mac OS X, there is nothing.  Dell uses their own BIOS stuff
and fan utilities won't recognize the fans.

So I set out and managed to combine the code from several
different sources for other platforms into a driver.

My laptop now runs at a cool 39°C instead of 60°C.

===========================================================
References
===========================================================

I8kfanGUI - the original inspiration for this project
http://www.diefer.de/i8kfan/

I8kfan DOS utility - assembly source code available
http://www.diefer.de/i8kfan/i8kfan.html

fan-0.4 - FreeBSD version of the driver
http://www.angelfire.com/punk4/bofn1001/

Linux 2.4.x driver
http://www.debian.org/~dz/i8k/

Linux 2.6.x driver
http://dellfand.dinglisch.net/

Dell Libsmbios project page
http://linux.dell.com/libsmbios/main/index.html
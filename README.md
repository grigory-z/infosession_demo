# INFOSESSION DEMO
Walkthrough for using LiME and Volatitlity to identify a malicious process, `hideme`,
which uses a helper kernel module to remove itself from the task list to resist
forensic analysis.

## Dependencies
This walkthrough assumes a fresh Xubuntu 16.04 LTS installation. Running this
demonstration in a VM is recommended to configure the RAM size, which is relevant
for LiME RAM images (1GB recommended).

Install the following dependencies from the Ubuntu repos:
```
$ sudo apt-get install
```

## WALKTHROUGH

### LiME
LiME (Linux Memory Extractor) allows for capturing RAM images that can be later
processed with memory forensics tools. It is built as a loadable kernel module (LKM)
which captures memory when loaded.

1. Build lime.ko
```
$ cd LiME/src
$ make
...
```

2. Load lime.ko to acquire RAM image. Unload after.
```
$ sudo insmod ./lime.ko "path=./ram.lime format=lime"
$ sudo rmmod lime.ko
```

3. Change ownership of image for easier use.
```
$ sudo chown user:user ./ram.lime
```

### Volatility
Volatility is a memory forensics suite. It can process RAM images generated
by LiME. It provides introspection capabilities on the runtime state at the
time of capture.

Although Volatility is packaged with a number of common profiles, it may be
necessary to generate one for the target kernel. This provides kernel symbol
and structure information needed to perform introspection on the RAM image.

Volatility supports many plugins for analysis; however this walkthrough only
covers the bare minimum required to read the process list from the RAM image.
See the Volatility docs for full installation instructions.

1. Run `python vol.py -h` for options
```
$ cd volatility
$ python vol.py -h
...
```

2. Generate a profile.
```
$ cd volatility/tools/linux
$ PROFILE=$(uname -n)-$(uname -r).zip
$ sudo make -C /lib/modules/$(uname -r)/build CONFIG_DEBUG_INFO=y M=$PWD modules
$ sudo chown user:user Module.symvers
$ dwarfdump -di ./module.o > module.dwarf
$ sudo zip $PROFILE module.dwarf /boot/System.map-$(uname -r)
$ cp $PROFILE ../../volatility/plugins/overlays/linux/
```

3. Run a command.
```
$ python vol.py -f ../LiME/src/ram.lime --profile=Linuxxubuntu-4_15_0-33-genericx64 linux_pslist
```

### hideme
The `hideme` userspace app and kernel module helper are provided as a target
for the memory forensics tools to find. The app can be started with no options
to act as a normal userspace process, or can leverage the kernel module to
remoe itself from the task list and hide itself in the Volatility process listing.

1. Build and load the kernel module to facilitate process hiding by removing from the task list.
```
$ cd hideme/kernel
$ make
...
$ sudo insmod hideme.ko
```

2. Build userspace hideme app.
```
$ cd hideme/userspace
$ make
...
```

3. Run hideme app without any hiding. Open a second terminal to use LiME to generate RAM image. Press ENTER to exit.
```
$ ./hideme
Press ENTER to exit

[second terminal]
$ sudo insmod ./lime.ko "path=./nohide.lime format=lime"
$ sudo rmmod lime.ko
```

4. Run hideme app with `-r` flag to remove self from task list via `hideme.ko`. Use LiME to generate RAM image. Press ENTER to exit.
```
$ ./hideme -r
removing myself from task list: 5388
Press ENTER to exit

[second terminal]
$ sudo insmod ./lime.ko "path=./hide.lime format=lime"
$ sudo rmmod lime.ko
```

5. Use volatility to analyze RAM images for the `hideme` process.
```
$ python vol.py -f ../LiME/src/nohide.lime --profile=Linuxxubuntu-4_15_0-33-genericx64 linux_pslist | grep hideme
...
0xffff8a18bc462d80 hideme               5465            5464            0               0      0x000000003445c000 2018-09-02 22:37:25 UTC+0000

$ python vol.py -f ../LiME/src/hide.lime --profile=Linuxxubuntu-4_15_0-33-genericx64 linux_pslist | grep hideme
...
```

### Cutter
Cutter is a reverse engineering GUI for the radare2 tool suite. It assists with
performing static analysis on executable binaries.

https://github.com/radareorg/cutter

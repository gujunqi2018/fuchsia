# Build and pave quickstart

This document shows you how to build and deploy Fuchsia on a target device with
`fx` development commands. Most of these commands have additional commands,
see `fx help <command>` for details.

## Determine USB drive device path {#usb-drive-device-path}

Before you attempt to build and pave Fuchsia on a target device, you need
to know the path of your USB drive.

Note: For either operating system, you can run the command once with the
USB drive disconnected, then run again with the USB drive connected, to see the
difference.

### fx

To determine the correct path to your USB drive:

Note: The `fx` tool is platform agnostic and lists available USB
drives.

```
fx mkzedboot
```

### Linux

To determine the correct path to your USB drive:

   ```
   sudo fdisk -l
   ```

   Drives are usually in the form `/dev/sd[x]` such as `/dev/sdc`.

   Make sure that you select the drive rather than a specific
   partition. For example, a specific partion has a number at the
   end of the path such as `/dev/sdc1`.

### macOS

To determine the correct path to your USB drive:

   ```
   diskutil list | grep external
   ```

   Drives are usually in the form `/dev/disk[n]` such as `/dev/disk2`.

   Note: If you see `ERROR: Can't open /dev/disk[n]: Resource busy`
   then you will have to unmount the USB drive. To do this, run:

   ```
   hdiutil unmount /dev/disk[n]
   ```

   If this does not fix the error, try reformating the drive:

   ```
   diskutil eraseDisk JHFSX <name_of_the_usb_stick> /dev/disk[n]
   ```

## Build and deploy Fuchsia

To perform an initial build and deploy of Fuchsia with `fx`, do the following:

1. Set your build type:

   Note: Configures the build to build the `core` product on a generic x64 board.
   For a list of available products and boards, see `fx list-products` and
   `fx list-boards` for lists of available products, respectively.

   ```
   fx set core.x64
   ```

1. Build a Fuchsia image:

   ```
   fx build
   ```

   This command builds Zircon and then the rest of Fuchsia.

1. Build the Zedboot media and install to a USB device target:

   Note: For information on obtainining the USB drive device path,
   see [USB drive device path](#usb-drive-device-path).

   ```
   fx mkzedboot <usb_drive_device_path>
   ```

1. Attach Zedboot USB drive to your target device and reboot that device.
1. On your target device, run:

   ```
   lsblk
   ```

1. Take note of the HDD or SSD's device path from the output of `lsblk`.
   An example path looks like `/dev/sys/pci/00:17.0/ahci/sata0/block`.
1. On your target device, run:

   ```
   install-disk-image init-partition-tables --block_device_path <BLOCK_DEVICE_PATH>
   ```

1. To start the bootserver, from your host, run:

   Note: The bootserver connects to the target device to upload the Fuchsia image
   and then paves your target device.

   ```
   fx pave
   ```

## Rebuild and redeploy Fuchsia

To rebuild and redeploy with `fx`:

1. Ensure that HEAD is in a good state to pull at
   the [build dashboard](https://luci-milo.appspot.com/p/fuchsia).
1. Fetch the latest code:

   ```
   jiri update
   ```

1. Build a Fuchsia image:

   ```
   fx build
   ```

   This command builds Zircon and then the rest of Fuchsia.
1. (Only for macOS users) Set up firewall rules:

   ```
   fx setup-macos
   ```
1. From your host, start a development package server:

   ```
   fx serve
   ```

1. Boot your target device without the Zedboot USB attached.
1. From your host, push updated Fuchsia packages to the target device:

   ```
   fx ota
   ```

   In some cases, if `fx ota` does not complete successfully, consider repaving
   with `fx pave`.

## Troubleshooting

*  If `fx build` fails, make sure that your `PATH` environment
   variable is set correctly.

   Note: The `fx` script changes the working directory in a way
   that may create conflicts between the commands it uses (such as `touch`)
   and the binaries in the working directory.

   To check the value of your `PATH` variable:

   ```
   echo $PATH
   ```

   Make that sure that the output of your `PATH` variable is a list of
   directories seperated by colons. Make sure that none of the
   directories are seperated by `.`.

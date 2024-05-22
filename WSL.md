# NextFOAM-2405 WSL distribution
NextFOAM-2405 WSL distribution image is provided.

Download `NextFOAM2405-ubuntu2204.zip` file from https://1drv.ms/u/s!AjRLfBSDK0kveTkM5t9DK3a3LdE?e=QZ98rZ and unzip it.

After unzip, install WSL image and execute using following commands

```
wsl.exe --import NextFOAM-2405 <directory for base disk> NextFOAM2405-ubuntu2204.vhdx --vhd
wsl.exe ~ -u nextfoam -d NextFOAM-2405
```

To execute the `gnome-terminal`, execute

```
wsl.exe ~ -u nextfoam -d NextFOAM-2405 dbus-launch gnome-terminal
```

The default user id and password are

```
id: nextfoam
pw: nextfoamcfd
```

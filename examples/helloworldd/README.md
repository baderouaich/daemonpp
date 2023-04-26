## Hello World Daemon
This daemon will log "Hello, World" to syslog every minute, forever, as long as your system is on.
[It will start when your system starts, and stops when your system stops](#enabledisable-daemon-with-system-startup).

## Build & Install
```bash 
mkdir build && cd build 
cmake .. -DCMAKE_BUILD_TYPE=Release
sudo make install
```

## Enable/Disable Daemon with system startup
```bash 
systemctl [enable|disable] helloworldd
```

## Start/Stop Daemon manually
```bash 
systemctl [start|stop] helloworldd
```

## Reload Daemon after config files updated
```bash 
systemctl reload helloworldd
```

## Restart Daemon
```bash 
systemctl restart helloworldd
```
> Equivalent to `stop systemctl stop helloworldd && systemctl start helloworldd` 

## Uninstall daemon 
```bash
# graceful stop daemon
systemctl stop helloworldd
# disable daemon with system startup
systemctl disable helloworldd
# remove daemon binary and config files
sudo make uninstall
```
> You can also manually remove files after you stop your daemon using: 
> ```bash
> rm -rf /etc/helloworldd/helloworldd.conf /etc/systemd/system/helloworldd.service /usr/bin/helloworldd
> ```
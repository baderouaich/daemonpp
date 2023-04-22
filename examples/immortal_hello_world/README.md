## Immortal Hello World Daemon
This daemon will log "Hello, World" to syslog every minute, forever, as long as your system is on.
It will start when your system starts, and stops when your system stops.

## Install
```bash 
mkdir build && cd build 
cmake .. -DCMAKE_BUILD_TYPE=Release
sudo make install
```

## Enable/Disable Daemon with system startup
```bash 
systemctl [enable|disable] immortal_hello_world
```

## Start/Stop Daemon manually
```bash 
systemctl [start|stop] immortal_hello_world
```

## Reload Daemon after config files updated
```bash 
systemctl reload immortal_hello_world
```

## Restart Daemon
```bash 
systemctl restart immortal_hello_world
```
> Equivalent to `stop systemctl stop immortal_hello_world && systemctl start immortal_hello_world` 

## Uninstall daemon 
```bash
# graceful stop daemon
systemctl stop immortal_hello_world
# disable daemon with system startup
systemctl disable immortal_hello_world
# remove daemon binary and config files
sudo make uninstall
```
> You can also manually remove files after you stop your daemon using: 
> ```bash
> rm -rf /etc/immortal_hello_world/immortal_hello_world.conf /etc/systemd/system/immortal_hello_world.service /usr/bin/immortal_hello_world
> ```
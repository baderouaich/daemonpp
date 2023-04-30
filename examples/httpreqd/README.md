## HTTP Request Daemon
This Daemon performs HTTP JSON request every 1 minute

## Build & Install
```bash 
mkdir build && cd build 
cmake .. -DCMAKE_BUILD_TYPE=Release
sudo make install
```

## Enable/Disable Daemon with system startup
```bash 
systemctl [enable|disable] httpreqd
```

## Start/Stop Daemon manually
```bash 
systemctl [start|stop] httpreqd
```

## Reload Daemon after config files updated
```bash 
systemctl reload httpreqd
```

## Restart Daemon
```bash 
systemctl restart httpreqd
```
> Equivalent to `stop systemctl stop httpreqd && systemctl start httpreqd` 

## Uninstall daemon 
```bash
# graceful stop daemon
systemctl stop httpreqd
# disable daemon with system startup
systemctl disable httpreqd
# remove daemon binary and config files
sudo make uninstall
```
> You can also manually remove files after you stop your daemon using: 
> ```bash
> rm -rf /etc/httpreqd/httpreqd.conf /etc/systemd/system/httpreqd.service /usr/bin/httpreqd
> ```
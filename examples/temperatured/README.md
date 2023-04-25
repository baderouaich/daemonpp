## Temperature Monitor Daemon
This daemon will monitor computer temperature every second and logs to a file in /tmp/temperatured.txt

## Build and Install
```bash 
mkdir build && cd build 
cmake .. -DCMAKE_BUILD_TYPE=Release
sudo make install
```

## Enable/Disable Daemon with system startup
```bash 
systemctl [enable|disable] temperatured
```

## Start/Stop Daemon manually
```bash 
systemctl [start|stop] temperatured
```

## Monitor temperatured output:
```bash
tail -f /tmp/temperatured.txt
```

## Reload Daemon after config files updated
```bash 
systemctl reload temperatured
```

## Restart Daemon
```bash 
systemctl restart temperatured
```
> Equivalent to `stop systemctl stop temperatured && systemctl start temperatured` 

## Uninstall daemon 
```bash
# graceful stop daemon
systemctl stop temperatured
# disable daemon with system startup
systemctl disable temperatured
# remove daemon binary and config files
sudo make uninstall
```
> You can also manually remove files after you stop your daemon using: 
> ```bash
> rm -rf /etc/temperatured/temperatured.conf /etc/systemd/system/temperatured.service /usr/bin/temperatured
> ```
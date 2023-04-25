Linux systems that use systemd for managing services, .service files are typically placed in the /lib/systemd/system/ directory or the /etc/systemd/system/ directory. The .conf files may be placed in a similar directory, such as /etc/my-daemon/.

### .service
```ini
# Properties docs: https://www.freedesktop.org/software/systemd/man/systemd.service.html
[Unit]
Description=Simple C++ template example for creating Linux daemons
After=network.target

[Service]
# Configures the process start-up type for this service unit. One of simple, exec, forking, oneshot, dbus, notify or idle.
# https://unix.stackexchange.com/questions/733890/systemd-service-unit-restart-on-failure-doesnt-restart-daemon
Type=forking
# when systemctl start is called
ExecStart=/usr/bin/daemonpp --config /etc/daemonpp/daemonpp.conf
# when systemctl reload my_daemon (for reloading of the service's configuration) it will trigger SIGHUP
# which will be caught by signal_handler and trigger the on_reload callback.
ExecReload=/bin/kill -s SIGHUP $MAINPID
# when systemctl stop my_daemon called: Will trigger SIGTERM which will be caught by signal_handler
# and trigger the on_stop callback.
ExecStop=/bin/kill -s SIGTERM $MAINPID
User=root
StandardError=syslog
SyslogIdentifier=daemonpp

[Install]
# Start after boot
WantedBy=multi-user.target
```


### .conf
```ini
# here you can have your daemon configuration
name=daemonpp
version=0.0.1
description=Simple C++ template example for creating Linux daemons
```

# NetworkMonitor test

These instructions are only for Linux and macOS.

Run the network-monitor integrated test binary, e.g.:
```
./build/tests/integrated/network-monitor
```
Note: sudo is not required.

You should see an `onInterfaceAdded` message for each ethernet and loopback
network interface present on the system, followed by an `onAddressAdded`
message for each IPv4/IPv6 address on each interface. Finally,
`onEnumerationCompleted` is printed, along with a summary of all interfaces
discovered thus far.

## Linux

[The following commands assume eth0 is the name of an ethernet interface
on the machine. If your interfaces are named differently, replace eth0
with the name of any ethernet interface that you have available.]

Command | Expected output
--------|----------------
sudo ip link add link eth0 name nmtest0 type vlan id 42 | `nmtest0: onInterfaceAdded`
sudo ip link set dev nmtest0 mtu 1342 | `nmtest0: onMtuChanged <old_mtu> -> 1342` (`old_mtu` is most likely 1500)
sudo ip link set dev nmtest0 up | `nmtest0: onStateChanged down -> <new_state>` (`new_state` is one of: running, dormant, no-carrier)
sudo ip address add 198.51.100.100/24 dev nmtest0 | `nmtest0: onAddressAdded 198.51.100.100/24`
sudo ip address del 198.51.100.100/24 dev nmtest0 | `nmtest0: onAddressRemoved 198.51.100.100/24`
sudo ip address add 2001:db8::1/80 dev nmtest0 | `nmtest0: onAddressAdded 2001:db8::1/80`
sudo ip address del 2001:db8::1/80 dev nmtest0 | `nmtest0: onAddressRemoved 2001:db8::1/80`
sudo ip link delete dev nmtest0 | `nmtest0: onInterfaceRemoved`

If you unplug the ethernet cable from your network card, you should see:
```
eth0: onStateChanged running -> no-carrier
nmtest0: onStateChanged running -> no-carrier
```

Plugging the cable back in should produce the following messages:
```
eth0: onStateChanged no-carrier -> running
nmtest0: onStateChanged no-carrier -> running
```

## macOS

[The following commands assume en0 is the name of an ethernet interface
on the machine. If your interfaces are named differently, replace en0
with the name of any ethernet interface that you have available.]

Command | Expected output
--------|----------------
sudo ifconfig vlan1 create | `vlan1: onInterfaceAdded`
sudo ifconfig vlan1 vlan 1 vlandev en0 | `vlan1: onStateChanged down -> running`
sudo ifconfig vlan1 198.51.100.100/24 | `vlan1: onAddressAdded 198.51.100.100/24`
sudo ifconfig vlan1 198.51.100.100/24 remove | `vlan1: onAddressRemoved 198.51.100.100/24`
sudo ifconfig vlan1 inet6 2001:db8::1/80 | `vlan1: onAddressAdded 2001:db8::1/80` (and potentially link-local addresses)
sudo ifconfig vlan1 inet6 2001:db8::1/80 remove | `vlan1: onAddressRemoved 2001:db8::1/80`
sudo ifconfig vlan1 destroy | `vlan1: onInterfaceRemoved`

If you unplug the ethernet cable from your network card, you should see:
```
en6: onStateChanged running -> down
```

Plugging the cable back in should produce the following messages:
```
en6: onStateChanged down -> running
```

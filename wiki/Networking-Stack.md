# Networking Stack

Full in-kernel TCP/IP stack: RTL8139 → Ethernet → ARP → IP → ICMP/UDP/TCP → DNS/HTTP.

## RTL8139 driver (`rtl8139.c`)
PCI NIC, I/O + MMIO BARs, 4 TX descriptors, circular RX buffer, link detection.

## ARP (`arp.c`)
16-entry cache with TTL. Sends "who has" broadcasts, handles replies, responds to requests.

## IP (`ip.c`)
IPv4 send/receive with header checksum. Routes off-subnet via gateway, delivers loopback internally.

## ICMP (`icmp.c`)
Responds to echo requests (type 8) with replies (type 0). `ping` CLI sends 4 requests with RTT/loss stats.

## UDP (`udp.c`)
Raw datagram send + port-based listener registration.

## TCP (`tcp.c`)
Full state machine: CLOSED → SYN_SENT → ESTABLISHED → FIN_WAIT → TIME_WAIT → CLOSED. 8 connections.

- **listen/accept:** Passive open for TCP servers
- **Retransmission (RTO):** Exponential backoff 300 ms → 2400 ms cap, 5 retries max
- **Loopback:** 127.0.0.x via software ring (no NIC needed)

## DHCP (`dhcp.c`)
DISCOVER → OFFER → REQUEST → ACK. Sets local_ip, netmask, gateway.

## DNS (`dns.c`)
Single A-record UDP query to configured server.

## HTTP (`http.c`)
HTTP/1.1 GET with TCP, Host header, timeout-based receive.

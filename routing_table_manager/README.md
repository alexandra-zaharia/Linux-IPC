# Routing table manager

The server process maintains a list of routing records that it synchronizes with connected clients via UNIX domain sockets.

## Routing record structure

A record of the routing table has the following fields:

* `destination` -- IPv4 address of the destination subnet
* `mask` -- integer value designating the subnet mask 
* `gateway` -- IPv4 address of the gateway 
* `oif` -- outgoing interface

## Routing table operations 

The routing table manager accepts the three following operations:

* **create** `destination`/`mask` `gateway` `oif` -- has the effect of creating a new record
* **update** `destination`/`mask` `gateway` `oif` -- has the effect of updating the specified destination subnet with a new gateway and/or outgoing interface
* **delete** `destination`/`mask` -- has the effect of removing the specified record

## Synchronization with clients

* Whenever a new client connects to the routing table manager, the client receives the whole contents of the routing table via UNIX domain sockets.
* Whenever the server creates, updates, or deletes a record, the clients' local copies of the routing table reflect this change.

## Extra

* Input validation for IPv4 addresses and subnet masks
* A record that already exists in the routing table cannot be added
* Only existing records may be updated/deleted
* When a client disconnects (Ctrl+C), the server recovers
* When the server diconnects, the clients also disconnect

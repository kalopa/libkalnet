# libkalnet

A C library for managing and maintaining TCP/IP connections in a
portable, efficient and non-blocking way.
After writing the same C code for TCP/IP connectivity many, many
times, it seemed right to just do it once more but this time as a
library.

The intention here is to have a basic library for either a server
or a client.
In the server case, you can listen for UDP packets or TCP connections.
The library will manage the new connections and (if required) handle
reads and writes.
You can specify callbacks for things like new connections or incoming
data.
Eventually, I'd like to build a pretty basic HTTP server with similar
callbacks for specific URLs.
The next progression after that would be to build in a Prometheus
client so you can report metrics to Prometheus without needing a
whole pile of additional code, just this library.
For now though, it's just about managing and maintaining channels
and connections.

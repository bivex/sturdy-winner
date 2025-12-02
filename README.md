# Libreactor - Extreme HTTP Performance Server

Optimized HTTP server based on libreactor with extreme performance.

## 🚀 Quick Start

```bash
# Compile with optimizations
./compile.sh

# Start the server
./run-optimized.sh

# Check status
./status.sh

# Stop the server
./stop.sh
```

## 📊 Benchmarking

```bash
# Quick test
wrk -t8 -c512 -d10s http://localhost:2342/plaintext

# Full benchmark
/var/www/benchmark-libreactor.sh
```

## ⚡ Performance Optimizations

### Application Level Code
- **SO_REUSEPORT + BPF filter** - connection distribution across CPUs
- **Busy Poll (SO_BUSY_POLL)** - low latency for network operations
- **TCP_NODELAY** - disabling Nagle algorithm
- **SO_KEEPALIVE = 0** - disabling keepalive for performance
- **Multi-process architecture** - process per CPU with CPU pinning

### Compilation
- `-O3 -march=native -flto` - maximum optimizations
- `-DNDEBUG -fomit-frame-pointer -funroll-loops` - additional optimizations

### System Level
- **Kernel parameters**: `nospectre_v1 nospectre_v2 pti=off mds=off tsx_async_abort=off`
- **Network sysctl**: 16MB buffers, busy poll, TCP optimizations
- **Nftables** instead of iptables (minimal overhead)

## 📁 Project Structure

```
/var/www/rads/
├── build/                     # Build directory (generated during compilation)
├── src/                       # Source code
│   ├── domain/                # HTTP domain logic
│   │   ├── http_response.c
│   │   └── http_server.c
│   ├── include/               # Header files
│   │   ├── compat/           # Compatibility headers
│   │   │   ├── dynamic.h
│   │   │   └── reactor.h
│   │   ├── domain/           # Domain headers
│   │   │   ├── http_response.h
│   │   │   └── http_server.h
│   │   ├── infrastructure/   # Infrastructure headers
│   │   │   └── server_infrastructure.h
│   │   └── platform/         # Platform headers
│   │       ├── log.h
│   │       ├── process.h
│   │       ├── signals.h
│   │       ├── socket.h
│   │       └── system.h
│   ├── infrastructure/        # Server infrastructure
│   │   └── server_infrastructure.c
│   ├── main/                  # Main application files
│   │   ├── libreactor-server.c
│   │   └── libreactor.c
│   └── platform/              # Platform utilities
│       ├── log.c
│       ├── process.c
│       ├── signals.c
│       ├── socket.c
│       └── system.c
├── compile.sh                 # Compilation with optimizations
├── run-optimized.sh          # Start with CPU pinning
├── stop.sh                   # Stop and cleanup
├── status.sh                 # Check status
├── Makefile                  # Alternative makefile
├── drop_changes.sh           # Git changes reset
├── fast_commits.sh           # Fast commits
├── switch_branch.sh          # Branch switching
├── git-init.sh               # Git repo initialization
├── libreactor-server.dockerfile # Dockerfile for server
├── libreactor.dockerfile     # Dockerfile for libreactor
└── README.md                 # This file
```

## 🎯 Performance

- **44k+ req/sec** on plaintext (3 CPUs, KVM virtualization, local test)
- **41k+ req/sec** on JSON responses (3 CPUs, KVM virtualization)
- **CPU spent on sendto()** (useful work)
- **Minimal locks and context switches**

## 🔧 API

### Endpoints
- `GET /plaintext` - returns "Hello, World!"
- `GET /json` - returns `{"message":"Hello, World!"}`

### Example Request
```bash
curl http://localhost:2342/plaintext
# Hello, World!

curl http://localhost:2342/json
# {"message":"Hello, World!"}
```

## 🛠️ Development

### Recompilation
```bash
make clean
make CFLAGS="-O3 -march=native -flto -DNDEBUG" libreactor-server
```

### Debug Build
```bash
make CFLAGS="-O0 -g" libreactor-server
```

## 📈 Monitoring

### CPU Profiling
```bash
perf record -F 99 -g -p $(pgrep libreactor-server | head -1) -o perf.data -- sleep 10
perf report -i perf.data
```

### System Calls
```bash
bpftrace -e 'tracepoint:syscalls:sys_enter_sendto { @[comm] = count(); } interval:s:1 { print(@); clear(@); }'
```

## 🔗 Links

- [Libreactor](https://github.com/fredrikwidlund/libreactor)
- [Extreme HTTP Performance Tuning](https://talawah.io/blog/extreme-http-performance-tuning-one-point-two-million/)
- [SO_REUSEPORT](https://lwn.net/Articles/542629/)

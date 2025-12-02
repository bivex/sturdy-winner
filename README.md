# Libreactor - Extreme HTTP Performance Server

Оптимизированный HTTP сервер на базе libreactor с экстремальной производительностью.

## 🚀 Быстрый старт

```bash
# Скомпилировать с оптимизациями
./compile.sh

# Запустить сервер
./run-optimized.sh

# Проверить статус
./status.sh

# Остановить сервер
./stop.sh
```

## 📊 Бенчмаркинг

```bash
# Быстрый тест
wrk -t8 -c512 -d10s http://localhost:2342/plaintext

# Полный бенчмарк
/var/www/benchmark-libreactor.sh
```

## ⚡ Оптимизации производительности

### Код уровня приложения
- **SO_REUSEPORT + BPF фильтр** - распределение соединений по CPU
- **Busy Poll (SO_BUSY_POLL)** - низкая latency для сетевых операций
- **TCP_NODELAY** - отключение Nagle алгоритма
- **SO_KEEPALIVE = 0** - отключение keepalive для производительности
- **Мультипроцессная архитектура** - процесс на CPU с CPU pinning

### Компиляция
- `-O3 -march=native -flto` - максимальные оптимизации
- `-DNDEBUG -fomit-frame-pointer -funroll-loops` - дополнительные оптимизации

### Системный уровень
- **Параметры ядра**: `nospectre_v1 nospectre_v2 pti=off mds=off tsx_async_abort=off`
- **Сетевые sysctl**: 16MB буферы, busy poll, TCP оптимизации
- **Nftables** вместо iptables (минимальный overhead)

## 📁 Структура проекта

```
/var/www/rads/
├── libreactor-server          # Оптимизированный бинарь
├── compile.sh                 # Компиляция с оптимизациями
├── run-optimized.sh          # Запуск с CPU pinning
├── stop.sh                   # Остановка и очистка
├── status.sh                 # Проверка статуса
├── benchmark_config.json      # Конфиг бенчмаркинга
├── src/                       # Исходный код
│   ├── libreactor-server.c   # Основной сервер
│   ├── helpers.c             # Вспомогательные функции
│   └── helpers.h
└── README.md
```

## 🎯 Производительность

- **65k-80k req/sec** на plaintext (3 CPU, виртуализация)
- **78k+ req/sec** на JSON responses
- **CPU тратится на sendto()** (полезная работа)
- **Минимальные блокировки и контекст-свитчи**

## 🔧 API

### Endpoints
- `GET /plaintext` - возвращает "Hello, World!"
- `GET /json` - возвращает `{"message":"Hello, World!"}`

### Пример запроса
```bash
curl http://localhost:2342/plaintext
# Hello, World!

curl http://localhost:2342/json
# {"message":"Hello, World!"}
```

## 🛠️ Разработка

### Перекомпиляция
```bash
make clean
make CFLAGS="-O3 -march=native -flto -DNDEBUG" libreactor-server
```

### Отладочная сборка
```bash
make CFLAGS="-O0 -g" libreactor-server
```

## 📈 Мониторинг

### CPU профилирование
```bash
perf record -F 99 -g -p $(pgrep libreactor-server | head -1) -o perf.data -- sleep 10
perf report -i perf.data
```

### Системные вызовы
```bash
bpftrace -e 'tracepoint:syscalls:sys_enter_sendto { @[comm] = count(); } interval:s:1 { print(@); clear(@); }'
```

## 🔗 Ссылки

- [Libreactor](https://github.com/fredrikwidlund/libreactor)
- [Extreme HTTP Performance Tuning](https://talawah.io/blog/extreme-http-performance-tuning-one-point-two-million/)
- [SO_REUSEPORT](https://lwn.net/Articles/542629/)

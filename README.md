# 🧭 HTTP Proxy Server (C++ | Boost.Beast | Qt | LRU Cache)

Этот проект реализует асинхронный HTTP-прокси-сервер на C++ с использованием **Boost.Beast 1.88**, поддерживающий:
- Асинхронную обработку клиентских HTTP-запросов
- Форвардинг и модификацию заголовков запроса
- Кэширование ответов от сервера через **LRU-кэш**

---

## 📦 Зависимости

- C++17
- [Boost 1.88+](https://www.boost.org/)
- CMake 3.16+

---

## 🔧 Сборка

```bash
git clone https://github.com/your-username/http-proxy.git
cd http-proxy
mkdir build
cd build
make
./http-proxy
```

---

## 🚀 Запуск

Простой запуск:

```bash
./http_proxy
```

Прокси слушает по умолчанию `localhost:8080`. Используйте `curl` или браузер с указанием прокси:

```bash
curl -x http://localhost:8080 http://example.com/
```

---

## 📁 Структура проекта

```
http-proxy/
├── CMakeLists.txt
├── main.cpp
├── proxy_server.h / .cpp         # основной класс сессии и логики
├── lru_cache.h                   # реализация кэша
```

---

## ⚙️ Возможности

- Многопоточность с `boost::asio::io_context`
- Автоматическое разрешение DNS
- Кэширование по URI (ключ — `target()` запроса)
- Обработка заголовков и возможная модификация (в точке forward)

---

## 🧪 Проверка

```bash
curl -v -x http://localhost:8080 http://example.com/
```

Если ответ пришел — повторный запрос будет извлечён из кэша.

---



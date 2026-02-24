# Installations-Anleitung (Deutsch)

Diese Anleitung beschreibt die Installation der Abhängigkeiten für das PhotoGallery Backend unter **Ubuntu 24.04 (Noble Numbat)** oder neuer.

## 1. System Update

```bash
sudo apt update && sudo apt upgrade -y
```

## 2. Grundlegende Build-Tools

```bash
sudo apt install -y build-essential cmake git pkg-config
```

## 3. Bibliotheken & Abhängigkeiten

### Allgemeine Bibliotheken

```bash
sudo apt install -y libssl-dev libjsoncpp-dev uuid-dev zlib1g-dev libspdlog-dev nlohmann-json3-dev libpq-dev libexiv2-dev libwebp-dev libargon2-dev
```

### Drogon Framework

Obwohl einige Ubuntu-Versionen `libdrogon-dev` anbieten, wird das Bauen aus dem Source-Code für vollen C++23 Support empfohlen:

```bash
git clone https://github.com/drogonframework/drogon
cd drogon
git submodule update --init
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
```

## 4. H2O Webserver

```bash
sudo apt install -y h2o
```

## 5. PostgreSQL

```bash
sudo apt install -y postgresql postgresql-contrib
```

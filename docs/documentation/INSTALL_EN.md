# Installation Guide (English)

This guide describes how to install the necessary dependencies for the PhotoGallery Backend on **Ubuntu 24.04 (Noble Numbat)** or newer.

## 1. System Update
```bash
sudo apt update && sudo apt upgrade -y
```

## 2. Essential Build Tools
```bash
sudo apt install -y build-essential cmake git pkg-config
```

## 3. Libraries & Dependencies

### General Libraries
```bash
sudo apt install -y 
    libssl-dev 
    libjsoncpp-dev 
    uuid-dev 
    zlib1g-dev 
    libspdlog-dev 
    nlohmann-json3-dev 
    libpq-dev 
    libexiv2-dev 
    libwebp-dev 
    libargon2-dev
```

### Drogon Framework
While some Ubuntu versions have `libdrogon-dev`, building from source is recommended for full C++23 support:
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

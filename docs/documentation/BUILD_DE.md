# Build-Anweisungen (Deutsch)

Folgen Sie diesen Schritten, um das PhotoGallery Backend zu kompilieren und auszuführen.

## 1. Umgebung vorbereiten
Kopieren Sie die Beispiel-Konfiguration und passen Sie die Werte an:
```bash
cp .env.example .env
# Bearbeiten Sie die .env Datei
nano .env
```

## 2. Kompilieren
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## 3. Datenbank-Setup
Initialisieren Sie das PostgreSQL-Schema:
```bash
psql -h localhost -U dein_user -d deine_db -f ../src/infra/db/schema.sql
```

## 4. Backend ausführen
```bash
./PhotoGalleryBackend
```

## 5. Verwendung des Import-Tools
Um Fotos aus einem Verzeichnis zu importieren:
```bash
./gallery-import /pfad/zu/deinen/fotos
```

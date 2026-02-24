# DEV-Umgebung Setup (Deutsch)

Diese Anleitung beschreibt die Einrichtung der lokalen Entwicklungsumgebung mit Docker und `docker-compose`.

## 1. Voraussetzungen

- Docker und Docker Compose installiert.
- Host-Verzeichnis: `/home/docker_user/Webapp/Data/web-gallery`.

## 2. Ordnerstruktur auf dem Host

Stellen Sie sicher, dass die folgende Struktur im Host-Volume existiert, damit H2O und das Backend die Dateien finden:

```bash
# Auf Ihrem Host-System
mkdir -p /home/docker_user/Webapp/Data/web-gallery/frontend
mkdir -p /home/docker_user/Webapp/Data/web-gallery/photos
mkdir -p /home/docker_user/Webapp/Data/web-gallery/thumbs
mkdir -p /home/docker_user/Webapp/Data/web-gallery/logs
```

## 3. Konfiguration (.env)

Kopieren Sie die Beispiel-Datei und passen Sie die Datenbank-Einstellungen an das `docker-compose.yml` an:

```bash
cp .env.example .env
```

Wichtig: Setzen Sie `DB_HOST=db` in der `.env`, damit die interne Kommunikation über das `docker-net` Docker-Netzwerk funktioniert.

## 4. Starten der Umgebung

Führen Sie den folgenden Befehl im Hauptverzeichnis aus:

```bash
docker-compose up --build
```

## 5. Zugriff auf die Anwendung

- **H2O Webserver (Frontend/Statisch)**: `http://localhost:8080`
- **Drogon API (Backend)**: `http://localhost:8848/api`
- **PostgreSQL**: `localhost:5432`

## 6. Import-Tool im Container nutzen

Um den Importer für Fotos im Volume auszuführen:

```bash
docker-compose exec backend ./gallery-import /data/photos
```

- **Stoppen**: `docker-compose stop` (hält ihn an, löscht ihn aber nicht)
- **Wieder starten**: `docker-compose start`
- **Komplett abbauen**: `docker-compose down` (stoppt ihn und löscht den Container, aber deine Daten im Volume unter /home/docker_user/... bleiben sicher erhalten!)

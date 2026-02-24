# PostgreSQL

**quick + dirty für DEV**

## Dockerfile

```dockerfile
# Verwende das offizielle, aktuelle PostgreSQL-Image
FROM postgres:latest

# Setze die Umgebungsvariablen (entspricht den -e Parametern)
ENV POSTGRES_PASSWORD=Sidolin1
ENV PGDATA=/var/lib/postgresql/data/pgdata

# Dokumentiere den internen Port (optional, aber Best Practice)
EXPOSE 5432
``
## Build

```bash
docker build -t mein_postgres_image .
```

## docker-compose

```yaml
version: '3.8'

services:
  db:
    image: postgres:latest
    container_name: psql_server
    hostname: psql_db
    environment:
      POSTGRES_PASSWORD: Sidolin1
      PGDATA: /var/lib/postgresql/data/pgdata
    ports:
      - "6543:5432"
    volumes:
      - /home/docker_user/Webapp/var/lib/postgresql/data:/var/lib/postgresql/data
    networks:
      - docker-net

networks:
  docker-net:
    external: true # Wir nehmen an, dass du das Netzwerk 'docker-net' bereits manuell erstellt hast
```

### Build

```bash
docker compose up build -d
```

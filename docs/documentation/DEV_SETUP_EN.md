# Development Environment Setup (English)

This guide describes how to set up the local development environment using Docker and `docker-compose`.

## 1. Prerequisites
- Docker and Docker Compose installed.
- Host directory: `/home/docker_user/Webapp/Data/web-gallery`.

## 2. Directory Structure on Host
Ensure the following structure exists in your host volume to allow H2O and the Backend to find files:
```bash
# On your Host System
mkdir -p /home/docker_user/Webapp/Data/web-gallery/frontend
mkdir -p /home/docker_user/Webapp/Data/web-gallery/photos
mkdir -p /home/docker_user/Webapp/Data/web-gallery/thumbs
mkdir -p /home/docker_user/Webapp/Data/web-gallery/logs
```

## 3. Configuration (.env)
Copy the example file and adjust the database settings to match `docker-compose.yml`:
```bash
cp .env.example .env
```
Ensure `DB_HOST=db` in your `.env` for internal networking via the `docker-net` Docker network.

## 4. Starting the Environment
Run the following command in the root directory:
```bash
docker-compose up --build
```

## 5. Accessing the Application
- **H2O Webserver (Frontend/Static)**: `http://localhost:8080`
- **Drogon API (Backend)**: `http://localhost:8848/api`
- **PostgreSQL**: `localhost:5432`

## 6. Using the Import Tool inside Docker
To run the importer for photos already in your volume:
```bash
docker-compose exec backend ./gallery-import /data/photos
```

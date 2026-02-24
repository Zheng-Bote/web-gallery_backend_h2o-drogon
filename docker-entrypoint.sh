#!/bin/bash
# docker-entrypoint.sh

# Start the H2O webserver in the background
h2o -c /etc/h2o/h2o.dev.conf &

# Start the Drogon Backend
# Ensure .env is present or variables are set in docker-compose/env-file
./PhotoGalleryBackend

-- SPDX-FileComment: PostgreSQL Database and Role Setup
-- SPDX-FileType: SOURCE
-- SPDX-FileContributor: ZHENG Robert
-- SPDX-FileCopyrightText: 2026 ZHENG Robert
-- SPDX-License-Identifier: Apache-2.0

-- 1. Create the Database
-- Run this as superuser (e.g., 'postgres')
CREATE DATABASE gallery;

-- 2. Create Roles
-- CRUD User (App Backend)
CREATE USER gallery_user WITH PASSWORD 'supersecret';

-- Read-Only User (Reporting / Analytics)
CREATE USER gallery_reader WITH PASSWORD 'readonlysecret';

-- 3. Basic Connection Privileges
GRANT CONNECT ON DATABASE gallery TO gallery_user;
GRANT CONNECT ON DATABASE gallery TO gallery_reader;

-- Note: Tables are created in schema.sql. 
-- Permissions are granted there to ensure new tables are covered.

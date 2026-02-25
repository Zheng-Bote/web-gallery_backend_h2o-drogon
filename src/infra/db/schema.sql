-- SPDX-FileComment: PostgreSQL Schema for Photo Gallery
-- SPDX-FileType: SOURCE
-- SPDX-FileContributor: ZHENG Robert
-- SPDX-FileCopyrightText: 2026 ZHENG Robert
-- SPDX-License-Identifier: Apache-2.0

-- Extension for UUID generation
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- ============================================================
-- USERS & RBAC
-- ============================================================

CREATE TABLE IF NOT EXISTS roles (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    name TEXT UNIQUE NOT NULL,
    description TEXT
);

CREATE TABLE IF NOT EXISTS permissions (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    name TEXT UNIQUE NOT NULL,
    description TEXT
);

CREATE TABLE IF NOT EXISTS role_permissions (
    role_id UUID REFERENCES roles(id) ON DELETE CASCADE,
    permission_id UUID REFERENCES permissions(id) ON DELETE CASCADE,
    PRIMARY KEY (role_id, permission_id)
);

CREATE TABLE IF NOT EXISTS users (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    username TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    totp_secret TEXT,
    is_active BOOLEAN DEFAULT TRUE,
    pwd_must_change BOOLEAN DEFAULT FALSE,
    language TEXT DEFAULT 'en',
    last_login_at TIMESTAMPTZ,
    created_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS user_roles (
    user_id UUID REFERENCES users(id) ON DELETE CASCADE,
    role_id UUID REFERENCES roles(id) ON DELETE CASCADE,
    PRIMARY KEY (user_id, role_id)
);

CREATE TABLE IF NOT EXISTS communication_channels (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id UUID REFERENCES users(id) ON DELETE CASCADE,
    channel_type TEXT NOT NULL, -- email, telegram, pushover
    enabled BOOLEAN DEFAULT TRUE,
    address TEXT NOT NULL,
    created_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP
);

-- ============================================================
-- LOCATIONS & PHOTOS
-- ============================================================

CREATE TABLE IF NOT EXISTS locations (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    continent TEXT,
    country TEXT,
    province TEXT,
    city TEXT,
    UNIQUE(continent, country, province, city)
);

CREATE TABLE IF NOT EXISTS photos (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    location_id UUID REFERENCES locations(id) ON DELETE SET NULL,
    file_name TEXT NOT NULL,
    file_path TEXT NOT NULL UNIQUE,
    thumb_path TEXT,
    width INT,
    height INT,
    taken_at TIMESTAMPTZ,
    camera_make TEXT,
    camera_model TEXT,
    lens TEXT,
    iso INT,
    aperture FLOAT,
    shutter TEXT,
    focal_length FLOAT,
    gps_lat DOUBLE PRECISION,
    gps_lon DOUBLE PRECISION,
    gps_alt DOUBLE PRECISION,
    is_public BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS photo_tags (
    photo_id UUID REFERENCES photos(id) ON DELETE CASCADE,
    tag TEXT NOT NULL,
    PRIMARY KEY (photo_id, tag)
);

CREATE TABLE IF NOT EXISTS photo_metadata_exif (
    photo_id UUID REFERENCES photos(id) ON DELETE CASCADE,
    key TEXT NOT NULL,
    value TEXT,
    PRIMARY KEY (photo_id, key)
);

CREATE TABLE IF NOT EXISTS photo_metadata_iptc (
    photo_id UUID REFERENCES photos(id) ON DELETE CASCADE,
    key TEXT NOT NULL,
    value TEXT,
    PRIMARY KEY (photo_id, key)
);

CREATE TABLE IF NOT EXISTS photo_metadata_xmp (
    photo_id UUID REFERENCES photos(id) ON DELETE CASCADE,
    key TEXT NOT NULL,
    value TEXT,
    PRIMARY KEY (photo_id, key)
);

-- Indexes for performance
CREATE INDEX idx_photos_location ON photos(location_id);
CREATE INDEX idx_photos_taken_at ON photos(taken_at);
CREATE INDEX idx_locations_hierarchy ON locations(continent, country, province, city);

-- ============================================================
-- PERMISSIONS / GRANTS
-- ============================================================

-- Grant CRUD permissions to the application user
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO gallery_user;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO gallery_user;

-- Grant Read-Only permissions to the reader user
GRANT SELECT ON ALL TABLES IN SCHEMA public TO gallery_reader;

-- Ensure future tables also get these permissions
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON TABLES TO gallery_user;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT SELECT ON TABLES TO gallery_reader;


# ER-Diagramm

```mermaid
erDiagram

    %% ============================
    %% USERS
    %% ============================
    users {
        UUID id PK
        TEXT username
        TEXT password_hash  "Argon2 hash"
        TEXT totp_secret
        BOOLEAN is_active
        BOOLEAN pwd_must_change
        TEXT language  "en | de"
        TIMESTAMPTZ last_login_at
        TIMESTAMPTZ created_at
        TIMESTAMPTZ updated_at
    }

    %% ============================
    %% ROLES
    %% ============================
    roles {
        UUID id PK
        TEXT name  "Admin | Editor | Reader | ..."
        TEXT description
    }

    %% ============================
    %% USER <-> ROLES (N:N)
    %% ============================
    user_roles {
        UUID user_id FK
        UUID role_id FK
    }

    %% ============================
    %% PERMISSIONS
    %% ============================
    permissions {
        UUID id PK
        TEXT name  "photo.read | photo.write | user.manage | ..."
        TEXT description
    }

    %% ============================
    %% ROLE <-> PERMISSIONS (N:N)
    %% ============================
    role_permissions {
        UUID role_id FK
        UUID permission_id FK
    }

    %% ============================
    %% COMMUNICATION CHANNELS
    %% ============================
    communication_channels {
        UUID id PK
        UUID user_id FK
        TEXT channel_type  "email | telegram | pushover | ..."
        BOOLEAN enabled
        TEXT address  "email address, telegram id, pushover key"
        TIMESTAMPTZ created_at
    }

    %% ============================
    %% LOCATIONS (Geo-Hierarchie)
    %% ============================
    locations {
        UUID id PK
        TEXT continent
        TEXT country
        TEXT province
        TEXT city
    }

    %% ============================
    %% PHOTOS
    %% ============================
    photos {
        UUID id PK
        UUID location_id FK
        TEXT file_name
        TEXT file_path
        TEXT thumb_path
        INT width
        INT height
        TIMESTAMPTZ taken_at
        TEXT camera_make
        TEXT camera_model
        TEXT lens
        INT iso
        FLOAT aperture
        TEXT shutter
        FLOAT focal_length
        DOUBLE gps_lat
        DOUBLE gps_lon
        TIMESTAMPTZ created_at
    }

    %% ============================
    %% TAGS
    %% ============================
    photo_tags {
        UUID photo_id FK
        TEXT tag
    }

    %% ============================
    %% METADATA TABLES
    %% ============================
    photo_metadata_exif {
        UUID photo_id FK
        TEXT key
        TEXT value
    }

    photo_metadata_iptc {
        UUID photo_id FK
        TEXT key
        TEXT value
    }

    photo_metadata_xmp {
        UUID photo_id FK
        TEXT key
        TEXT value
    }

    %% ============================
    %% RELATIONSHIPS
    %% ============================

    users ||--o{ user_roles : "has roles"
    roles ||--o{ user_roles : "assigned to users"

    roles ||--o{ role_permissions : "has permissions"
    permissions ||--o{ role_permissions : "assigned to roles"

    users ||--o{ communication_channels : "has channels"

    locations ||--o{ photos : "has many"
    photos ||--o{ photo_tags : "has many"
    photos ||--o{ photo_metadata_exif : "has many"
    photos ||--o{ photo_metadata_iptc : "has many"
    photos ||--o{ photo_metadata_xmp : "has many"
```

## 🧭 Beziehungsübersicht

### 1. users

Unabhängig von den restlichen Tabellen

Enthält Login‑Daten, TOTP‑Secret, Rollen

| Feld                   | Zweck                        |
| ---------------------- | ---------------------------- |
| password_hash          | Argon2‑Hash, sicher & modern |
| last_login_at          | Audit & Security             |
| is_active              | User sperren/entsperren      |
| pwd_must_change        | Passwort‑Reset erzwingen     |
| language               | UI‑Sprache (en/de)           |
| totp_secret            | 2FA/TOTP                     |
| created_at, updated_at | Audit‑Felder                 |

## 🛡️ 2. Rollen & Berechtigungen (RBAC)

Du bekommst ein vollständiges Role‑Based Access Control‑System:

### Rollen (Admin, Editor, Reader, …)

- beliebig erweiterbar
- mehrere Rollen pro User möglich

### Permissions

- granular (z. B. photo.read, photo.write, user.manage)
- Rollen können beliebig viele Permissions haben

**Vorteile:**

- extrem flexibel
- ideal für Enterprise‑Backends
- perfekt für Drogon‑Middleware

## 📣 3. Kommunikationskanäle

Tabelle communication_channels:

| Feld         | Beispiel                            |
| ------------ | ----------------------------------- |
| channel_type | email, telegram, pushover           |
| enabled      | true/false                          |
| address      | E‑Mail, Telegram‑ID, Pushover‑Token |

**Damit kannst du:**

- Benachrichtigungen senden
- Kanäle aktivieren/deaktivieren
- später WebPush, SMS, Matrix, etc. ergänzen

## 4. locations

- Modelliert die Geo‑Hierarchie:
  - continent
  - country
  - province
  - city
- Ein Eintrag pro vollständiger Hierarchie
- **1 location → n photos**

## 5. photos

- Enthält:
  - Dateipfade
  - Thumbnail‑Pfad
  - Kamera‑Daten
  - EXIF‑Basiswerte
  - GPS‑Koordinaten
  - Zeitstempel
- **FK: location_id → locations.id**

## 6. photo_tags

- Einfaches Tagging‑System
- **n tags pro photo**

## 7. Metadaten‑Tabellen

- EXIF, IPTC, XMP getrennt
- Jeweils:
  - photo_id
  - key
  - value
- Vorteil:
  - flexibel
  - beliebig erweiterbar
  - keine Schema‑Änderungen nötig

# API Documentation (English)

This documentation describes the available API endpoints for the Photo Gallery Backend application.

## Authentication (Auth)

| URL | Method | Auth / Roles | Description |
|:--- |:--- |:--- |:--- |
| `/api/login` | POST | Public | First login step: Password verification. Returns success status or TOTP requirement. |
| `/api/login/totp` | POST | Public | Second login step: TOTP code verification and JWT token issuance. |

## Locations

| URL | Method | Auth / Roles | Description |
|:--- |:--- |:--- |:--- |
| `/api/locations/tree` | GET | Optional | Returns the location hierarchy tree. Unauthenticated users see only locations associated with public photos. |

## Photos

| URL | Method | Auth / Roles | Description |
|:--- |:--- |:--- |:--- |
| `/api/photos` | GET | Optional | Lists available photos. Supports pagination (`limit`, `offset`). Unauthenticated users see only public photos. |
| `/api/photos/{id}` | GET | Optional | Returns detailed information for a specific photo. Unauthenticated users can only access public photos. |
| `/api/ping` | GET | Public | Simple API health check. Returns "alive". |

## User

| URL | Method | Auth / Roles | Description |
|:--- |:--- |:--- |:--- |
| `/api/users/me` | GET | Authenticated | Returns the profile of the currently logged-in user (ID, username, permissions, channels). |

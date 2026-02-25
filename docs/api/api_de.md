# API Dokumentation (Deutsch)

Diese Dokumentation beschreibt die verfügbaren API-Endpunkte der Photo Gallery Backend-Anwendung.

## Authentifizierung (Auth)

| URL | Methode | Auth / Rollen | Beschreibung |
|:--- |:--- |:--- |:--- |
| `/api/login` | POST | Öffentlich | Erster Schritt der Anmeldung: Passwortprüfung. Gibt Erfolgsstatus oder TOTP-Anforderung zurück. |
| `/api/login/totp` | POST | Öffentlich | Zweiter Schritt der Anmeldung: Verifizierung des TOTP-Codes und Rückgabe des JWT-Tokens. |

## Standorte (Locations)

| URL | Methode | Auth / Rollen | Beschreibung |
|:--- |:--- |:--- |:--- |
| `/api/locations/tree` | GET | Optional | Gibt den Hierarchiebaum aller Standorte zurück. Nicht authentifizierte Nutzer sehen nur Standorte mit öffentlichen Fotos. |

## Fotos (Photos)

| URL | Methode | Auth / Rollen | Beschreibung |
|:--- |:--- |:--- |:--- |
| `/api/photos` | GET | Optional | Listet verfügbare Fotos auf. Unterstützt Paging (`limit`, `offset`). Nicht authentifizierte Nutzer sehen nur öffentliche Fotos. |
| `/api/photos/{id}` | GET | Optional | Gibt Detailinformationen zu einem spezifischen Foto zurück. Zugriff für nicht authentifizierte Nutzer nur auf öffentliche Fotos. |
| `/api/ping` | GET | Öffentlich | Einfacher Gesundheitscheck der API. Gibt "alive" zurück. |

## Benutzer (User)

| URL | Methode | Auth / Rollen | Beschreibung |
|:--- |:--- |:--- |:--- |
| `/api/users/me` | GET | Authentifiziert | Gibt das Profil des aktuell angemeldeten Benutzers zurück (ID, Name, Berechtigungen, Kanäle). |

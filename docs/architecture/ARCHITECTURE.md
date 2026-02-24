# Architecture Documentation

## 🏗️ Design Philosophy
The backend follows **Clean Architecture** principles to ensure maintainability, testability, and independence from external libraries.

- **Domain Layer**: Contains business logic and entities. No dependencies on frameworks.
- **Infrastructure Layer**: Implements repository interfaces using PostgreSQL and utility tools.
- **API Layer**: Handles HTTP requests, controllers, and middleware using Drogon.

## 📊 Component Diagram

```mermaid
flowchart LR
    Angular["Client (Angular)"]
    H2O["H2O (Proxy)"]
    API["Drogon (API)"]
    DB[("PostgreSQL")]
    FS["File System"]

    Angular <--> H2O
    H2O <--> API
    H2O <--> FS
    API <--> DB
```

## 🔐 Security Flow (Sequence)

```mermaid
sequenceDiagram
    participant U as User
    participant M as AuthMiddleware
    participant S as TokenService
    participant C as Controller

    U->>M: Request with Bearer Token
    M->>S: verify_token(token)
    alt Valid
        S-->>M: UserPayload
        M->>C: Invoke Action
        C-->>U: 200 OK
    else Invalid
        S-->>M: std::unexpected
        M-->>U: 401 Unauthorized
    end
```

## 📂 Directory Structure
- `src/app`: Application entry points (`main.cpp`, `import_main.cpp`).
- `src/api`: Controllers and Middlewares.
- `src/core`: Core services like Logging and Config.
- `src/domain`: Business logic, Models, and Interfaces.
- `src/infra`: Database repositories and utility scripts.

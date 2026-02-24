# Build Instructions (English)

Follow these steps to compile and run the PhotoGallery Backend.

## 1. Prepare Environment
Copy the example environment file and adjust the values:
```bash
cp .env.example .env
# Edit .env with your favorite editor
nano .env
```

## 2. Compile
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## 3. Database Setup
Initialize the PostgreSQL schema:
```bash
psql -h localhost -U your_user -d your_db -f ../src/infra/db/schema.sql
```

## 4. Run the Backend
```bash
./PhotoGalleryBackend
```

## 5. Using the Import Tool
To import photos from a directory:
```bash
./gallery-import /path/to/your/photos
```

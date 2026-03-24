-- ============================================================
-- Lisa Lashes — database schema
-- Runs automatically on first container start via
-- /docker-entrypoint-initdb.d/
-- ============================================================

-- 1. users
CREATE TABLE IF NOT EXISTS users (
    id            SERIAL PRIMARY KEY,
    first_name    TEXT NOT NULL,
    last_name     TEXT NOT NULL,
    phone         TEXT UNIQUE NOT NULL,
    address       TEXT,
    password_hash TEXT NOT NULL
    is_admin      BOOLEAN DEFAULT FALSE
);

-- 2. sessions
CREATE TABLE IF NOT EXISTS sessions (
    id          SERIAL PRIMARY KEY,
    user_id     INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    token_hash  TEXT NOT NULL UNIQUE,
    expires_at  TIMESTAMPTZ NOT NULL DEFAULT NOW() + INTERVAL '24 hours'
);

-- 3. products
CREATE TABLE IF NOT EXISTS products (
    id          SERIAL PRIMARY KEY,
    name        TEXT NOT NULL,
    description TEXT,
    price       NUMERIC(10,2) NOT NULL,
    stock       INTEGER NOT NULL DEFAULT 0,
    category    TEXT
);

-- 4. carts
CREATE TABLE IF NOT EXISTS carts (
    id         SERIAL PRIMARY KEY,
    user_id    INTEGER NOT NULL UNIQUE REFERENCES users(id) ON DELETE CASCADE,
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

-- 5. cart_items
CREATE TABLE IF NOT EXISTS cart_items (
    id         SERIAL PRIMARY KEY,
    cart_id    INTEGER NOT NULL REFERENCES carts(id) ON DELETE CASCADE,
    product_id INTEGER NOT NULL REFERENCES products(id) ON DELETE CASCADE,
    quantity   INTEGER NOT NULL DEFAULT 1,
    UNIQUE (cart_id, product_id)
);

-- 6. orders
CREATE TABLE IF NOT EXISTS orders (
    id         SERIAL PRIMARY KEY,
    user_id    INTEGER NOT NULL REFERENCES users(id),
    total      NUMERIC(10,2) NOT NULL,
    status     TEXT NOT NULL DEFAULT 'pending',
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

-- 7. order_items
CREATE TABLE IF NOT EXISTS order_items (
    id                SERIAL PRIMARY KEY,
    order_id          INTEGER NOT NULL REFERENCES orders(id) ON DELETE CASCADE,
    product_id        INTEGER NOT NULL REFERENCES products(id),
    price_at_purchase NUMERIC(10,2) NOT NULL,
    quantity          INTEGER NOT NULL
);

-- 8. salon_services
CREATE TABLE IF NOT EXISTS salon_services (
    id       SERIAL PRIMARY KEY,
    name     TEXT NOT NULL,
    duration INTERVAL NOT NULL,
    price    NUMERIC(10,2) NOT NULL,
    active   BOOLEAN NOT NULL DEFAULT TRUE
);

-- 9. resources
CREATE TABLE IF NOT EXISTS resources (
    id     SERIAL PRIMARY KEY,
    name   TEXT NOT NULL,
    active BOOLEAN NOT NULL DEFAULT TRUE
);

-- 10. resource_services
CREATE TABLE IF NOT EXISTS resource_services (
    resource_id INTEGER NOT NULL REFERENCES resources(id) ON DELETE CASCADE,
    service_id  INTEGER NOT NULL REFERENCES salon_services(id) ON DELETE CASCADE,
    PRIMARY KEY (resource_id, service_id)
);

-- 11. working_hours
CREATE TABLE IF NOT EXISTS working_hours (
    id          SERIAL PRIMARY KEY,
    resource_id INTEGER NOT NULL REFERENCES resources(id) ON DELETE CASCADE,
    weekday     SMALLINT NOT NULL CHECK (weekday BETWEEN 0 AND 6),
    start_time  TIME NOT NULL,
    end_time    TIME NOT NULL,
    UNIQUE (resource_id, weekday)
);

-- 12. schedule_overrides
CREATE TABLE IF NOT EXISTS schedule_overrides (
    id          SERIAL PRIMARY KEY,
    resource_id INTEGER NOT NULL REFERENCES resources(id) ON DELETE CASCADE,
    date        DATE NOT NULL,
    working     BOOLEAN NOT NULL DEFAULT TRUE,
    start_time  TIME,
    end_time    TIME,
    note        TEXT,
    UNIQUE (resource_id, date)
);

-- 13. bookings
DO $$ BEGIN
    CREATE TYPE booking_status AS ENUM ('pending', 'confirmed', 'cancelled', 'completed');
EXCEPTION WHEN duplicate_object THEN NULL;
END $$;

CREATE TABLE IF NOT EXISTS bookings (
    id             SERIAL PRIMARY KEY,
    resource_id    INTEGER NOT NULL REFERENCES resources(id) ON DELETE CASCADE,
    customer_name  TEXT NOT NULL,
    customer_phone TEXT,
    customer_email TEXT,
    start_time     TIMESTAMPTZ NOT NULL,
    end_time       TIMESTAMPTZ NOT NULL,
    status         booking_status NOT NULL DEFAULT 'confirmed',
    service_id     INTEGER REFERENCES salon_services(id),
    created_at     TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

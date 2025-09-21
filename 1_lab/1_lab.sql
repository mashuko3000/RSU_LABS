CREATE DATABASE t01_library;

CREATE TABLE public.author(
	author_id SERIAL PRIMARY KEY,
	last_name VARCHAR(256) NOT NULL,
	first_name VARCHAR(256) NOT NULL
);

CREATE TABLE public.publishing_house (
    publishing_house_id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    city VARCHAR(255)
);

CREATE TABLE public.book (
    book_id SERIAL PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    author_id INTEGER REFERENCES public.author(author_id),
    publishing_house_id INTEGER REFERENCES public.publishing_house(publishing_house_id),
    edition INTEGER,
    publication_year INTEGER,
    circulation INTEGER
);

CREATE TABLE public.reader (
    reader_id VARCHAR(20) PRIMARY KEY,
    last_name VARCHAR(255) NOT NULL,
    first_name VARCHAR(255) NOT NULL,
    birth_date DATE,
    gender CHAR(1) CHECK (gender IN ('M', 'F')),
    registration_date DATE
);

CREATE TABLE public.book_instance (
    instance_id SERIAL PRIMARY KEY,
    book_id INTEGER REFERENCES public.book(book_id),
    state VARCHAR(50) CHECK (state IN ('отличное', 'хорошее', 'удовлетворительное', 'ветхое', 'утеряна')),
    status VARCHAR(50) CHECK (status IN ('в наличии', 'выдана', 'забронирована')),
    location VARCHAR(255)
);

CREATE TABLE public.issuance (
    reader_id VARCHAR(20) REFERENCES public.reader(reader_id),
    instance_id INTEGER REFERENCES public.book_instance(instance_id),
    issue_date TIMESTAMP NOT NULL,
    expected_return_date TIMESTAMP NOT NULL,
    actual_return_date TIMESTAMP,
    PRIMARY KEY (reader_id, instance_id)
);

CREATE TABLE public.booking (
    booking_id SERIAL PRIMARY KEY,
    reader_id VARCHAR(20) REFERENCES public.reader(reader_id),
    book_id INTEGER REFERENCES public.book(book_id),
    min_instance_state VARCHAR(50) CHECK (min_instance_state IN ('отличное', 'хорошее', 'удовлетворительное', 'ветхое', 'утеряна')),
    booking_date TIMESTAMP NOT NULL
);

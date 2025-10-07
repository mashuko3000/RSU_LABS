CREATE TABLE public.logs(
	log_id BIGSERIAL PRIMARY KEY,
	log_data_time TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT NOW(),
	log_context VARCHAR(100) NOT NULL,
	log_content TEXT
);
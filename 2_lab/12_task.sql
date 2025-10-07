CREATE OR REPLACE PROCEDURE cancel_book_reservation(
	p_reader_card_number INTEGER,
	p_book_id INTEGER
)
AS $$
DECLARE 
	v_booking_number INTEGER;
	v_inventory_number INTEGER;
BEGIN
	SELECT 
		b.booking_number, bi.inventory_number
	INTO
		v_booking_number, v_inventory_number
	FROM 
		public.booking b
	JOIN
		public.booking_instance bi ON b.book_id = bi_book_id
	WHERE
		b.reader_card_number = p_reader_card_number
		AND b.book_id = p_book_id
		AND bi.status = 'booked'
	ORDER BY
		b.booking_datetime DESC
	LIMIT 1
	FOR UPDATE;

	IF NOT FOUND THEN
		RAISE EXCEPTION 'Actived booking of ID % book for reader % cant be founded', p_book_id, p_reader_card_number;
	END IF;

	DELETE FROM public.booking
	WHERE booking_number = v_booking_number;

	UPDATE public.book_instance
	SET status = 'in stock'
	WHERE inventory_number = v_inventory_number;

END;
$$ LANGUAGE plpgsql;
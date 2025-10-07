CREATE OR REPLACE PROCEDURE book_an_instance(
    p_reader_card_number INTEGER,
    p_book_id INTEGER,
    p_minimum_state book_state
)
AS $$
DECLARE
    v_inventory_number INTEGER;
    v_expiration_date DATE;
BEGIN
    v_expiration_date := CURRENT_DATE + INTERVAL '2 days';

    SELECT inventory_number INTO v_inventory_number
    FROM public.book_instance
    WHERE book_id = p_book_id
      AND status = 'in stock'
      AND state >= p_minimum_state
    ORDER BY state ASC
    LIMIT 1
    FOR UPDATE;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Нет доступных экземпляров книги ID % со статусом "in stock" и состоянием не хуже %.', p_book_id, p_minimum_state;
    END IF;

    INSERT INTO public.booking (
        reader_card_number,
        book_id,
        minimum_state,
        booking_datetime
    )
    VALUES (
        p_reader_card_number,
        p_book_id,
        p_minimum_state,
        NOW()
    );

    UPDATE public.book_instance
    SET status = 'booked'
    WHERE inventory_number = v_inventory_number;


END;
$$ LANGUAGE plpgsql;
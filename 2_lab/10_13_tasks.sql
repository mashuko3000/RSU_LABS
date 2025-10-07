CREATE OR REPLACE PROCEDURE issue_book_with_full_check(
    p_reader_card_number INTEGER,
    p_inventory_number INTEGER,
    p_expected_return_date DATE DEFAULT (CURRENT_DATE + INTERVAL '14 days')::DATE
)
AS $$
DECLARE
    v_has_overdue BOOLEAN;
    v_book_status book_status;
    v_reserved_reader_card_number INTEGER;
BEGIN
    SELECT EXISTS (
        SELECT 1 FROM overdue_books_info obi WHERE obi."ФИО Пользователя" = r.first_name || ' ' || r.last_name
        FROM public.reader r WHERE r.reader_card_number = p_reader_card_number
    ) INTO v_has_overdue;

    IF v_has_overdue THEN
        RAISE EXCEPTION 'Выдача запрещена! У читателя с билетом % есть просроченные книги.', p_reader_card_number;
    END IF;

    SELECT status INTO v_book_status
    FROM public.book_instance
    WHERE inventory_number = p_inventory_number
    FOR UPDATE;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Экземпляр книги с инвентарным номером % не найден.', p_inventory_number;
    END IF;


    IF v_book_status = 'issued' THEN
        RAISE EXCEPTION 'Выдача запрещена! Экземпляр уже выдан другому читателю.';
    END IF;


    IF v_book_status = 'booked' THEN
        SELECT
            b.reader_card_number INTO v_reserved_reader_card_number
        FROM
            public.booking b
        JOIN
            public.book_instance bi ON b.book_id = bi.book_id
        WHERE
            bi.inventory_number = p_inventory_number
        ORDER BY b.booking_datetime DESC
        LIMIT 1;

        IF v_reserved_reader_card_number IS NOT NULL AND v_reserved_reader_card_number <> p_reader_card_number THEN
            RAISE EXCEPTION 'Выдача запрещена! Эта книга забронирована за другим читателем (Карта №: %).', v_reserved_reader_card_number;
        END IF;

        IF v_reserved_reader_card_number = p_reader_card_number THEN
            DELETE FROM public.booking WHERE reader_card_number = p_reader_card_number AND book_id = (SELECT book_id FROM public.book_instance WHERE inventory_number = p_inventory_number);
        END IF;
    END IF;

    INSERT INTO public.issuance (
        reader_card_number,
        inventory_number,
        issue_datetime,
        expected_return_date,
        actual_return_date
    )
    VALUES (
        p_reader_card_number,
        p_inventory_number,
        NOW(),
        p_expected_return_date,
        NULL
    );

    UPDATE public.book_instance
    SET status = 'issued'
    WHERE inventory_number = p_inventory_number;

END;
$$ LANGUAGE plpgsql;

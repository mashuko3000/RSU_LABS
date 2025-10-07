CREATE OR REPLACE VIEW overdue_books_info AS
SELECT
	r.last_name || ' ' || r.first_name AS "User",
	a.last_name || ' ' || a.last_name AS "Author",
	b.title AS "Title",
	(NOW()::date - i.expected_return_date) AS "Overdue (days)"
FROM 
	public.issuance i
JOIN
    public.reader r ON i.reader_card_number = r.reader_card_number
JOIN
    public.book_instance bi ON i.inventory_number = bi.inventory_number
JOIN
    public.book b ON bi.book_id = b.id
JOIN
    public.author a ON b.author_id = a.id
WHERE
    i.actual_return_date IS NULL
    AND i.expected_return_date < CURRENT_DATE;
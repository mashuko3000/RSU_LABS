CREATE OR REPLACE VIEW long_overdue_books_view AS
SELECT
	r.first_name || ' ' || r.last_name AS "Reader",
	b.title AS "title",
	i.issue_datetime AS "Date",
	NOW() - i.issue_datetime AS "Time passed"
FROM
	public.issuance i
JOIN
	public.reader r ON i.reader_card_number = r.reader_card_number
JOIN
	public.book_instance bi ON i.inventory_number = bi.inventory_number
JOIN
	public.book b ON bi.book_id = b.id
WHERE
	i.actual_return_date IS NULL
	AND i.issue_datetime < NOW() - INTERVAL '1 year';
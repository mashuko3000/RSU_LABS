CREATE OR REPLACE VIEW issued_books_info AS
SELECT
	r.last_name || ' ' || r.first_name AS "User",
	a.last_name || ' ' || a.first_name AS "Author",
	b.title AS "title",
	bi.state AS "State",
	i.issue_datetime AS "Date"
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
    i.actual_return_date IS NULL;

SELECT * FROM issued_books_info;
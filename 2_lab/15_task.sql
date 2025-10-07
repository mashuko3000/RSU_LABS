CREATE OR REPLACE VIEW available_books_summary AS
SELECT 
	b.title AS "title",
	a.first_name || ' ' || a.last_name AS "Author",
	bi.state AS "State",
	COUNT(bi.inventory_number) AS "Count of items"
FROM
	public.book_instance bi
JOIN
	public.book b ON bi.book_id = b.id
JOIN
	public.author a ON b.author_id = a.id
WHERE 
	 bi.status = 'in stock'
GROUP BY
	b.title,
	a.first_name,
	a.last_name,
	bi.state
ORDER BY
	b.title,
	CASE bi.state
		WHEN 'excellent' THEN 1
		WHEN 'good' THEN 2
		WHEN 'satisfactory' THEN 3
		WHEN 'dilapidated' THEN 4
		WHEN 'lost' THEN 5
		ELSE 52
	END ASC;
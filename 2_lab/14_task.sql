CREATE OR REPLACE FUNCTION get_book_locations_by_state(p_book_id INTEGER)
RETURNING TABLE(
	inventory_number INTEGER,
	book_state book_state,
	location_details VARCHAR
)
AS $$
BEGIN
	RETURN QUERY
	SELECT 
		bi.inventory_number,
		bi.state,
		l.coppus || ', ' || l.hall || ', ' || l.shelf || ', ' || l.rack
	FROM
		public.book_instance bi
	JOIN
		public.location l ON bi.location_id = l.id
	WHERE
		bi.book_id = l.id
	ORDER BY
		CASE bi.state
			WHEN 'excellent' THEN 1
			WHEN 'good' THEN 2
			WHEN 'satisfying' THEN 3
			WHEN 'dilapidated' THEN 4
			WHEN 'lost' THEN 5
			ELSE 52
		END ASC,
		bi.inventory_number;
END
$$ LANGUAGE plpgsql;

SELECT * FROM get_book_locations_by_state(5);
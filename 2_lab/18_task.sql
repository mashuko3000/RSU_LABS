CREATE OR REPLACE FUNCTION log_dml_changes()
RETURNS TRIGGER AS $$
DECLARE 
	v_table_name TEXT := TG_TABLE_NAME;
	v_operation TEXT := TG_OP;
	v_log_content TEXT;
BEGIN
	IF v_operation = 'INSERT' THEN
		v_log_content := format('INSERT: Добавлена новая строка в таблицу %s. Содержимое : %s', v_table_name, ROW(NEW.*)::text);
	ELSEIF v_operation = 'UPDATE' THEN
		v_log_content := format('UPDATE: СТрока %s в таблице %s изменена. OLD : %s; NEW : %s', v_table_name, ROW(OLD.*)::text, ROW(NEW.*)::text);
	ELSEIF v_operation ='DELETE' THEN
		v_log_content := format('DELETE: Строка %s в таюлице %s была удалена', ROW(OLD.*)::text, v_table_name);
	ELSE
		RETURN NULL;
	END IF;

	INSERT INTO public.logs (log_context, log_content)
	VALUES (v_table_name, v_log_content);

	IF v_operation = 'DELETE' THEN
		RETURN OLD;
	ELSE
		RETURN NEW;
	END IF;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER author_dml_log
AFTER INSERT OR UPDATE OR DELETE ON public.author
FOR EACH ROW
EXECUTE FUNCTION log_dml_changes();

CREATE TRIGGER publishing_house_dml_log
AFTER INSERT OR UPDATE OR DELETE ON public.publishing_house
FOR EACH ROW
EXECUTE FUNCTION log_dml_changes();

CREATE TRIGGER book_dml_log
AFTER INSERT OR UPDATE OR DELETE ON public.book
FOR EACH ROW
EXECUTE FUNCTION log_dml_changes();

CREATE TRIGGER reader_dml_log
AFTER INSERT OR UPDATE OR DELETE ON public.reader
FOR EACH ROW
EXECUTE FUNCTION log_dml_changes();

CREATE TRIGGER location_dml_log
AFTER INSERT OR UPDATE OR DELETE ON public.location
FOR EACH ROW
EXECUTE FUNCTION log_dml_changes();

CREATE TRIGGER book_instance_dml_log
AFTER INSERT OR UPDATE OR DELETE ON public.book_instance
FOR EACH ROW
EXECUTE FUNCTION log_dml_changes();

CREATE TRIGGER issuance_dml_log
AFTER INSERT OR UPDATE OR DELETE ON public.issuance
FOR EACH ROW
EXECUTE FUNCTION log_dml_changes();

CREATE TRIGGER booking_dml_log
AFTER INSERT OR UPDATE OR DELETE ON public.booking
FOR EACH ROW
EXECUTE FUNCTION log_dml_changes();
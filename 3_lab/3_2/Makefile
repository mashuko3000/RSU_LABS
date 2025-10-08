CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
# -Wall: Включает все основные предупреждения
# -std=c99: Требует стандарт C99
# -g: Включает отладочную информацию

# Имена конечных исполняемых файлов
SERVER = server
CLIENT = client

# Цель 'all' (Цель по умолчанию)
# Зависит от двух исполняемых файлов
all: $(SERVER) $(CLIENT)

# Правило для компиляции server.c
$(SERVER): server.c
	$(CC) $(CFLAGS) server.c -o $(SERVER)

# Правило для компиляции client.c
$(CLIENT): client.c
	$(CC) $(CFLAGS) client.c -o $(CLIENT)

# Цель 'run' (Запуск)
# Сначала запускаем сервер, затем клиент
# Использование & позволяет запустить сервер в фоновом режиме
run: all
	@echo "--- Запуск Сервера в фоне (PID: $$!) ---"
	./$(SERVER) &
	SERVER_PID=$$!
	@echo "--- Запуск Клиента ---"
	./$(CLIENT)
	@echo "--- Ожидание завершения Сервера ---"
	wait $$SERVER_PID

# Цель 'clean' (Очистка)
# Удаляет скомпилированные исполняемые файлы и FIFO
clean:
	rm -f $(SERVER) $(CLIENT) $(FIFO_PATH)
	@echo "Cleaned up binaries and FIFO."

# Включаем путь к FIFO для цели clean
FIFO_PATH = /tmp/my_fifo_2526

.PHONY: all run clean
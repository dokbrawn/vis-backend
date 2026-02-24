import zmq
import time
from datetime import datetime


LOG_FILE = "android_messages.log"



def save_message_to_file(message):
    """Сохраняет пришедшее сообщение в файл с временной меткой."""
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    with open(LOG_FILE, "a", encoding="utf-8") as f:
        f.write(f"{timestamp} - {message}\n")

def read_all_saved_messages():
    """Читает и выводит все сообщения из файла."""
    try:
        with open(LOG_FILE, "r", encoding="utf-8") as f:
            print("\n--- Сохраненные сообщения ---")
            content = f.read()
            if not content:
                print("(файл пуст)")
            else:
                print(content.strip())
            print("--------------------------\n")
    except FileNotFoundError:
        print(f"\nФайл {LOG_FILE} еще не создан. Сообщений нет.\n")

# --- Основная логика сервера ---

def start_server():
    """Запускает ZMQ сервер типа REP."""
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    
    socket.bind("tcp://*:5556")
    
    print("Сервер запущен и слушает на порту 5556...")
    print("Нажмите Ctrl+C, чтобы остановить сервер и показать логи.")

    message_count = 0

    try:
        while True:
            message = socket.recv_string()
            message_count += 1
            print(f"Получено сообщение #{message_count}: {message}")
            save_message_to_file(message)
            time.sleep(1)
            response = f"Hello from Server! You are message #{message_count}."
            socket.send_string(response)
    
    except KeyboardInterrupt:
        print("\nСервер останавливается...")
    
    finally:
        socket.close()
        context.term()
        print("Сервер остановлен.")
        read_all_saved_messages()

if __name__ == "__main__":
    start_server()

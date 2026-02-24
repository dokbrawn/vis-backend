#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <chrono>


#include <zmq.hpp>           
#include <nlohmann/json.hpp>  


using json = nlohmann::json;


struct LocationData {
    std::mutex mtx;
    double latitude = 0.0;
    double longitude = 0.0;
    double altitude = 0.0;
    long long time = 0;
};


void run_server_thread(LocationData* shared_loc) {
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::rep);
    
    try {
        socket.bind("tcp://*:5556");
        std::cout << "[SERVER] Сервер запущен на порту 5556..." << std::endl;
    } catch (const zmq::error_t& e) {
        std::cerr << "[SERVER] Ошибка привязке порта: " << e.what() << std::endl;
        return;
    }

    std::ofstream json_log_file("locations_log.json", std::ios_base::app);

    while (true) {
        zmq::message_t request;
        try {
            
            socket.recv(request, zmq::recv_flags::none);
            std::string msg_str = request.to_string();
            std::cout << "[SERVER] Получено: " << msg_str << std::endl;

            
            json received_json = json::parse(msg_str);

            
            {
                std::lock_guard<std::mutex> lock(shared_loc->mtx);
                shared_loc->latitude = received_json.value("latitude", 0.0);
                shared_loc->longitude = received_json.value("longitude", 0.0);
                shared_loc->altitude = received_json.value("altitude", 0.0);
                shared_loc->time = received_json.value("time", 0LL);
            }

            
            if (json_log_file.is_open()) {
                json_log_file << received_json.dump() << std::endl;
            }

            
            socket.send(zmq::buffer("OK"), zmq::send_flags::none);

        } catch (const std::exception& e) {
            std::cerr << "[SERVER] Ошибка: " << e.what() << std::endl;
             
            if(socket.connected()) {
                 socket.send(zmq::buffer("Error"), zmq::send_flags::none);
            }
        }
    }
}


void run_gui_thread(LocationData* shared_loc) {
    std::cout << "[GUI] GUI-поток запущен." << std::endl;
    while (true) {
        
        std::this_thread::sleep_for(std::chrono::seconds(5));

        
        {
            std::lock_guard<std::mutex> lock(shared_loc->mtx);
            std::cout << "\n===== [GUI] Обновление данных =====" << std::endl;
            std::cout << "  Широта: " << shared_loc->latitude << std::endl;
            std::cout << "  Долгота: " << shared_loc->longitude << std::endl;
            std::cout << "  Высота:  " << shared_loc->altitude << std::endl;
            std::cout << "=================================\n" << std::endl;
        }
    }
}

int main() {
    static LocationData shared_location_data;

    std::cout << "[MAIN] Запускаем серверный и GUI потоки..." << std::endl;

    std::thread server_thread(run_server_thread, &shared_location_data);
    std::thread gui_thread(run_gui_thread, &shared_location_data);

    
    server_thread.join();
    gui_thread.join();

    return 0;
}
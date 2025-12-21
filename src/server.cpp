#include <httplib.h>
#include <json.hpp>
#include "storage.hpp"
#include <iostream>
#include <memory>
#include <ctime>

using json = nlohmann::json;
using namespace httplib;

void log_request(const std::string& method, const std::string& path, int status) {
    time_t now = time(nullptr);
    char timestr[26];
#ifdef _WIN32
    ctime_s(timestr, sizeof(timestr), &now);
#else
    ctime_r(&now, timestr);
#endif
    timestr[24] = '\0';
    std::cout << "[" << timestr << "] " << method << " " << path << " - " << status << std::endl;
}

int main() {
    Server svr;
    
    std::cout << "\n============================================================\n";
    std::cout << "  Password Vault REST API Server\n";
    std::cout << "============================================================\n";
    std::cout << "  Initializing server...\n";
    
    // Initialize storage manager
    auto storage = std::make_shared<StorageManager>("data/vault.dat", "data/users.dat");
    
    // CORS headers
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization, X-Username"}
    });
    
    // Handle OPTIONS requests for CORS preflight
    svr.Options(".*", [](const Request&, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Username");
        res.status = 200;
    });
    
    // Health check endpoint
    svr.Get("/api/health", [](const Request& req, Response& res) {
        json response = {{"status", "ok"}, {"server", "Password Vault API"}};
        res.set_content(response.dump(), "application/json");
        log_request("GET", req.path, 200);
    });
    
    // Register endpoint
    svr.Post("/api/register", [storage](const Request& req, Response& res) {
        try {
            auto body = json::parse(req.body);
            std::string username = body["username"];
            std::string password = body["password"];
            
            if (username.empty() || password.empty()) {
                json response = {{"success", false}, {"message", "Username and password required"}};
                res.set_content(response.dump(), "application/json");
                res.status = 400;
                log_request("POST", req.path, 400);
                return;
            }
            
            uint64_t user_id = storage->registerUser(username, password, "");
            
            if (user_id > 0) {
                json response = {{"success", true}, {"message", "Registration successful"}};
                res.set_content(response.dump(), "application/json");
                log_request("POST", req.path, 200);
                std::cout << "  [INFO] User registered: " << username << std::endl;
            } else {
                json response = {{"success", false}, {"message", "Username already exists"}};
                res.set_content(response.dump(), "application/json");
                res.status = 400;
                log_request("POST", req.path, 400);
            }
        } catch (const std::exception& e) {
            json response = {{"success", false}, {"message", "Server error"}};
            res.set_content(response.dump(), "application/json");
            res.status = 500;
            log_request("POST", req.path, 500);
            std::cerr << "  [ERROR] Registration failed: " << e.what() << std::endl;
        }
    });
    
    // Login endpoint
    svr.Post("/api/login", [storage](const Request& req, Response& res) {
        try {
            auto body = json::parse(req.body);
            std::string username = body["username"];
            std::string password = body["password"];
            
            if (username.empty() || password.empty()) {
                json response = {{"success", false}, {"message", "Username and password required"}};
                res.set_content(response.dump(), "application/json");
                res.status = 400;
                log_request("POST", req.path, 400);
                return;
            }
            
            std::string token = storage->loginUser(username, password);
            
            if (!token.empty()) {
                json response = {{"success", true}, {"sessionToken", token}};
                res.set_content(response.dump(), "application/json");
                log_request("POST", req.path, 200);
                std::cout << "  [INFO] User logged in: " << username << std::endl;
            } else {
                json response = {{"success", false}, {"message", "Invalid credentials"}};
                res.set_content(response.dump(), "application/json");
                res.status = 401;
                log_request("POST", req.path, 401);
            }
        } catch (const std::exception& e) {
            json response = {{"success", false}, {"message", "Server error"}};
            res.set_content(response.dump(), "application/json");
            res.status = 500;
            log_request("POST", req.path, 500);
            std::cerr << "  [ERROR] Login failed: " << e.what() << std::endl;
        }
    });
    
    // Get all passwords
    svr.Get("/api/passwords", [storage](const Request& req, Response& res) {
        std::string token = req.get_header_value("Authorization");
        
        if (token.empty()) {
            json response = {{"success", false}, {"message", "Unauthorized"}};
            res.set_content(response.dump(), "application/json");
            res.status = 401;
            log_request("GET", req.path, 401);
            return;
        }
        
        try {
            uint64_t user_id = storage->validateSession(token);
            
            if (user_id == 0) {
                json response = {{"success", false}, {"message", "Invalid session"}};
                res.set_content(response.dump(), "application/json");
                res.status = 401;
                log_request("GET", req.path, 401);
                return;
            }
            
            auto passwords = storage->getUserVault(user_id);
            
            json pwd_array = json::array();
            for (const auto& pwd : passwords) {
                json pwd_obj;
                pwd_obj["id"] = std::to_string(pwd.record_id);
                pwd_obj["site"] = pwd.site_name;
                pwd_obj["username"] = pwd.username;
                pwd_obj["password"] = pwd.password;
                pwd_obj["category"] = pwd.category;
                pwd_obj["notes"] = pwd.notes;
                pwd_array.push_back(pwd_obj);
            }
            
            json response;
            response["success"] = true;
            response["passwords"] = pwd_array;
            res.set_content(response.dump(), "application/json");
            log_request("GET", req.path, 200);
        } catch (const std::exception& e) {
            json response = {{"success", false}, {"message", "Server error"}};
            res.set_content(response.dump(), "application/json");
            res.status = 500;
            log_request("GET", req.path, 500);
            std::cerr << "  [ERROR] Get passwords failed: " << e.what() << std::endl;
        }
    });
    
    // Add password
    svr.Post("/api/passwords", [storage](const Request& req, Response& res) {
        std::string token = req.get_header_value("Authorization");
        
        if (token.empty()) {
            json response = {{"success", false}, {"message", "Unauthorized"}};
            res.set_content(response.dump(), "application/json");
            res.status = 401;
            log_request("POST", req.path, 401);
            return;
        }
        
        try {
            uint64_t user_id = storage->validateSession(token);
            
            if (user_id == 0) {
                json response = {{"success", false}, {"message", "Invalid session"}};
                res.set_content(response.dump(), "application/json");
                res.status = 401;
                log_request("POST", req.path, 401);
                return;
            }
            
            auto body = json::parse(req.body);
            
            std::string site = body["site"];
            std::string username = body["username"];
            std::string password = body["password"];
            std::string category = body.value("category", "");
            std::string notes = body.value("notes", "");
            
            if (site.empty() || username.empty() || password.empty()) {
                json response = {{"success", false}, {"message", "Site, username, and password required"}};
                res.set_content(response.dump(), "application/json");
                res.status = 400;
                log_request("POST", req.path, 400);
                return;
            }
            
            uint64_t record_id = storage->addVaultEntry(user_id, site, username, password, notes, category);
            
            json response;
            response["success"] = true;
            response["message"] = "Password added";
            response["id"] = std::to_string(record_id);
            res.set_content(response.dump(), "application/json");
            log_request("POST", req.path, 200);
            std::cout << "  [INFO] Password added: " << site << std::endl;
        } catch (const std::exception& e) {
            json response = {{"success", false}, {"message", "Server error"}};
            res.set_content(response.dump(), "application/json");
            res.status = 500;
            log_request("POST", req.path, 500);
            std::cerr << "  [ERROR] Add password failed: " << e.what() << std::endl;
        }
    });
    
    // Update password
    svr.Put(R"(/api/passwords/(.*))", [storage](const Request& req, Response& res) {
        std::string token = req.get_header_value("Authorization");
        
        if (token.empty()) {
            json response = {{"success", false}, {"message", "Unauthorized"}};
            res.set_content(response.dump(), "application/json");
            res.status = 401;
            log_request("PUT", req.path, 401);
            return;
        }
        
        try {
            uint64_t user_id = storage->validateSession(token);
            
            if (user_id == 0) {
                json response = {{"success", false}, {"message", "Invalid session"}};
                res.set_content(response.dump(), "application/json");
                res.status = 401;
                log_request("PUT", req.path, 401);
                return;
            }
            
            std::string password_id_str = req.matches[1];
            uint64_t record_id = std::stoull(password_id_str);
            auto body = json::parse(req.body);
            
            std::string site = body["site"];
            std::string username = body["username"];
            std::string password = body["password"];
            std::string category = body.value("category", "");
            std::string notes = body.value("notes", "");
            
            bool success = storage->updateVaultEntry(user_id, record_id, site, username, password, notes, category);
            
            if (success) {
                json response = {{"success", true}, {"message", "Password updated"}};
                res.set_content(response.dump(), "application/json");
                log_request("PUT", req.path, 200);
                std::cout << "  [INFO] Password updated: " << site << std::endl;
            } else {
                json response = {{"success", false}, {"message", "Password not found"}};
                res.set_content(response.dump(), "application/json");
                res.status = 404;
                log_request("PUT", req.path, 404);
            }
        } catch (const std::exception& e) {
            json response = {{"success", false}, {"message", "Server error"}};
            res.set_content(response.dump(), "application/json");
            res.status = 500;
            log_request("PUT", req.path, 500);
            std::cerr << "  [ERROR] Update password failed: " << e.what() << std::endl;
        }
    });
    
    // Delete password
    svr.Delete(R"(/api/passwords/(.*))", [storage](const Request& req, Response& res) {
        std::string token = req.get_header_value("Authorization");
        
        if (token.empty()) {
            json response = {{"success", false}, {"message", "Unauthorized"}};
            res.set_content(response.dump(), "application/json");
            res.status = 401;
            log_request("DELETE", req.path, 401);
            return;
        }
        
        try {
            uint64_t user_id = storage->validateSession(token);
            
            if (user_id == 0) {
                json response = {{"success", false}, {"message", "Invalid session"}};
                res.set_content(response.dump(), "application/json");
                res.status = 401;
                log_request("DELETE", req.path, 401);
                return;
            }
            
            std::string password_id_str = req.matches[1];
            uint64_t record_id = std::stoull(password_id_str);
            
            bool success = storage->deleteVaultEntry(user_id, record_id);
            
            if (success) {
                json response = {{"success", true}, {"message", "Password deleted"}};
                res.set_content(response.dump(), "application/json");
                log_request("DELETE", req.path, 200);
                std::cout << "  [INFO] Password deleted" << std::endl;
            } else {
                json response = {{"success", false}, {"message", "Password not found"}};
                res.set_content(response.dump(), "application/json");
                res.status = 404;
                log_request("DELETE", req.path, 404);
            }
        } catch (const std::exception& e) {
            json response = {{"success", false}, {"message", "Server error"}};
            res.set_content(response.dump(), "application/json");
            res.status = 500;
            log_request("DELETE", req.path, 500);
            std::cerr << "  [ERROR] Delete password failed: " << e.what() << std::endl;
        }
    });
    
    std::cout << "  Server ready!\n";
    std::cout << "============================================================\n";
    std::cout << "  Listening on: http://0.0.0.0:8080\n";
    std::cout << "  API Base URL: http://localhost:8080/api/\n";
    std::cout << "  Data Directory: data/\n";
    std::cout << "\n  Press Ctrl+C to stop the server\n";
    std::cout << "============================================================\n\n";
    
    svr.listen("0.0.0.0", 8080);
    
    return 0;
}

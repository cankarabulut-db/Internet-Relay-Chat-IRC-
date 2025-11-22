#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <termios.h>
#include <fcntl.h>
#include <poll.h>

// Terminal ayarlarını değiştir (raw mode için)
void enableRawMode(struct termios& orig_termios)
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);  // Echo ve canonical mode'u kapat
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// Terminal ayarlarını eski haline getir
void disableRawMode(struct termios& orig_termios)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// Ctrl+D'yi yeni satıra çeviren fonksiyon
void handleControlD()
{
    std::cout << std::endl;  // Yeni satıra geç
    std::cout.flush();
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <server_ip> <port>" << std::endl;
        return 1;
    }

    const char* server_ip = argv[1];
    int port = atoi(argv[2]);

    // Socket oluştur
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    // Sunucu adresini ayarla
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address" << std::endl;
        close(sock);
        return 1;
    }

    // Sunucuya bağlan
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Connection failed" << std::endl;
        close(sock);
        return 1;
    }

    std::cout << "Connected to " << server_ip << ":" << port << std::endl;
    std::cout << "Ctrl+D = Newline, Ctrl+C = Exit" << std::endl;
    std::cout << "======================================" << std::endl;

    struct termios orig_termios;
    enableRawMode(orig_termios);

    // Non-blocking socket
    fcntl(sock, F_SETFL, O_NONBLOCK);

    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = sock;
    fds[1].events = POLLIN;

    std::string input_buffer;
    char buf[4096];
    bool running = true;

    while (running)
    {
        int ret = poll(fds, 2, -1);
        if (ret < 0)
            break;

        // Klavyeden input
        if (fds[0].revents & POLLIN)
        {
            char c;
            ssize_t n = read(STDIN_FILENO, &c, 1);
            
            if (n > 0)
            {
                // Ctrl+D (EOT = 4)
                if (c == 4)
                {
                    handleControlD();  // Yeni satıra geç
                    std::cout << "> " << std::flush;
                }
                // Ctrl+C (ETX = 3)
                else if (c == 3)
                {
                    std::cout << "\n^C Exiting..." << std::endl;
                    running = false;
                }
                // Enter (newline)
                else if (c == '\n' || c == '\r')
                {
                    if (!input_buffer.empty())
                    {
                        // Komutları sunucuya gönder
                        input_buffer += "\r\n";
                        send(sock, input_buffer.c_str(), input_buffer.length(), 0);
                        input_buffer.clear();
                    }
                    std::cout << std::endl << "> " << std::flush;
                }
                // Backspace
                else if (c == 127 || c == 8)
                {
                    if (!input_buffer.empty())
                    {
                        input_buffer.erase(input_buffer.length() - 1);
                        std::cout << "\b \b" << std::flush;  // Karakteri sil
                    }
                }
                // Normal karakter
                else if (c >= 32 && c < 127)
                {
                    input_buffer += c;
                    std::cout << c << std::flush;
                }
            }
        }

        // Sunucudan gelen mesajlar
        if (fds[1].revents & POLLIN)
        {
            memset(buf, 0, sizeof(buf));
            ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
            
            if (n > 0)
            {
                // Mevcut satırı temizle
                std::cout << "\r\033[K";  // Satırı temizle
                std::cout << buf;
                std::cout << "> " << input_buffer << std::flush;  // Prompt'u yeniden yaz
            }
            else if (n == 0)
            {
                std::cout << "\nConnection closed by server" << std::endl;
                running = false;
            }
        }

        // Bağlantı hatası
        if (fds[1].revents & (POLLERR | POLLHUP))
        {
            std::cout << "\nConnection error" << std::endl;
            running = false;
        }
    }

    disableRawMode(orig_termios);
    close(sock);
    
    return 0;
}

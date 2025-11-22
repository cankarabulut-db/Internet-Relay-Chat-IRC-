# 🧪 IRC Sunucu Partial Data Manuel Test Rehberi

## Problem: Ctrl+D Çalışmıyor

`nc` ile Ctrl+D kullanmak her zaman işe yaramıyor. İşte alternatif yöntemler:

---

## ✅ YÖNTEM 1: Telnet Kullan (En Kolay)

```bash
telnet 127.0.0.1 6667
```

Sonra şunları YAZ (her satır sonrası Enter'a BAS):
```
PASS secret123
NICK testuser
USER test 0 * :Test User
PRIVMSG testuser :Hello
QUIT
```

---

## ✅ YÖNTEM 2: NC + Echo Kombinasyonu

Tek komutta çalıştır:

```bash
echo -e "PASS secret123\r\nNICK testuser\r\nUSER test 0 * :Test User\r\nQUIT\r\n" | nc 127.0.0.1 6667
```

Veya parçalı gönder:

```bash
(sleep 0.2; printf "PA"; sleep 0.2; printf "SS secret123\r\n"; sleep 0.2; printf "NICK test\r\n"; sleep 1; printf "QUIT\r\n") | nc 127.0.0.1 6667
```

---

## ✅ YÖNTEM 3: NC İnteraktif + Dosya

1. Komutları bir dosyaya yaz:
```bash
cat > commands.txt << 'EOF'
PASS secret123
NICK testuser
USER test 0 * :Test User
JOIN #test
PRIVMSG #test :Hello World
QUIT
EOF
```

2. Dosyayı nc'ye besle:
```bash
nc 127.0.0.1 6667 < commands.txt
```

---

## ✅ YÖNTEM 4: Hazır Script (Otomatik)

Ben sizin için hazırladım:

```bash
./manual_test.sh
```

Bu script:
- Her komutu parçalara böler
- Aralarına gecikme ekler
- Sunucunun cevaplarını gösterir

---

## 🔍 Partial Data'yı Nasıl Görebilirsiniz?

### A) Sunucu Loglarına Ekleyin

`Server.cpp` dosyasına debug ekleyin:

```cpp
// handleClientData() fonksiyonunda
clientBuffers[clientSocket] += buffer;
std::cout << "DEBUG: Received partial: '" << buffer << "'" << std::endl;
std::cout << "DEBUG: Full buffer now: '" << clientBuffers[clientSocket] << "'" << std::endl;
```

### B) Wireshark ile İzleyin

```bash
sudo wireshark
# Filtre: tcp.port == 6667
```

---

## 🎯 Subject'teki Gerçek Test

Subject'te istenen:

```bash
nc -C 127.0.0.1 6667
com^Dman^Dd
```

**Açıklama:**
- `^D` = Ctrl+D (flush but don't close)
- Ama Linux'ta bu pek çalışmaz

**Linux'ta Eşdeğeri:**

```bash
{ printf "com"; sleep 0.2; printf "man"; sleep 0.2; printf "d\r\n"; } | nc 127.0.0.1 6667
```

Veya:

```bash
strace -e write nc 127.0.0.1 6667
# Sonra manuel yaz: comd (Enter)
# Göreceksin ki her karakter ayrı write() ile gidiyor
```

---

## 📊 Sunucu Loglarını İzle

Bir terminal:
```bash
./ircserv 6667 secret123
```

Başka terminal:
```bash
./manual_test.sh
```

İlk terminalde partial data mesajlarını göreceksin!

---

## ✨ Hızlı Test Komutları

### Test 1: Basit Bağlantı
```bash
echo -e "PASS secret123\r\nQUIT\r\n" | nc 127.0.0.1 6667
```

### Test 2: Parçalı PASS
```bash
(printf "PA"; sleep 0.1; printf "SS secret123\r\n"; sleep 0.5; printf "QUIT\r\n") | nc 127.0.0.1 6667
```

### Test 3: Tam IRC Akışı
```bash
{
  printf "PASS secret123\r\n"
  sleep 0.2
  printf "NICK testuser\r\n"
  sleep 0.2
  printf "USER test 0 * :Real Name\r\n"
  sleep 0.5
  printf "JOIN #testchannel\r\n"
  sleep 0.3
  printf "PRIVMSG #testchannel :Hello everyone!\r\n"
  sleep 0.3
  printf "QUIT :Goodbye\r\n"
} | nc 127.0.0.1 6667
```

---

## 🐛 Debug Modu için Kod Eklentisi

Eğer görmek istersen, `Server.cpp`'ye ekle:

```cpp
// handleClientData() içinde, while (true) döngüsünde:
if (bytesRead > 0)
{
    buffer[bytesRead] = '\0';
    std::cout << "📦 Partial recv [" << clientSocket << "]: '" 
              << buffer << "' (" << bytesRead << " bytes)" << std::endl;
    clientBuffers[clientSocket] += buffer;
    std::cout << "💾 Buffer now [" << clientSocket << "]: '" 
              << clientBuffers[clientSocket] << "'" << std::endl;
}
```

---

## ✅ Sonuç

Kodunuz **ZATEN** partial data'yı doğru işliyor! 

Test için en kolay yöntem:
```bash
./manual_test.sh
```

Veya interaktif için:
```bash
telnet 127.0.0.1 6667
```

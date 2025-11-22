# 🎮 Interactive IRC Client - Ctrl+D Desteği

## Özellikler

✅ **Ctrl+D** - Yeni satıra geçer (cursor bir alt satıra atılır)  
✅ **Ctrl+C** - Client'tan çıkar  
✅ **Backspace** - Karakterleri siler  
✅ **Enter** - Komutu sunucuya gönderir  
✅ Real-time sunucu mesajları görüntüleme  

---

## Derleme

```bash
make
```

Bu hem `ircserv` (sunucu) hem de `ircclient` (client) derler.

---

## Kullanım

### 1. Sunucuyu Başlat

```bash
./ircserv 6667 secret123
```

### 2. Client'ı Başlat

```bash
./ircclient 127.0.0.1 6667
```

### 3. Komut Gönder

Client açıldığında otomatik olarak prompt gösterir:

```
Connected to 127.0.0.1:6667
Ctrl+D = Newline, Ctrl+C = Exit
======================================
> 
```

Şimdi komutları yazabilirsin:

```
> PASS secret123
> NICK testuser
> USER test 0 * :Test User
> JOIN #channel
> PRIVMSG #channel :Hello World!
> QUIT
```

---

## Ctrl+D Özelliği

**Normal kullanım:**
```
> PASS secret123[ENTER]
```

**Ctrl+D ile:**
```
> PASS sec[Ctrl+D]
> ret123[ENTER]
```

Ctrl+D basıldığında:
- Cursor bir alt satıra atılır
- Yeni bir `>` prompt gösterilir
- Yazdığın şey kaybolmaz, devam edebilirsin

---

## Özel Tuşlar

| Tuş | Fonksiyon |
|-----|-----------|
| `Ctrl+D` | Yeni satıra geç |
| `Ctrl+C` | Client'tan çık |
| `Enter` | Komutu gönder |
| `Backspace` | Son karakteri sil |

---

## Örnek Kullanım

### Terminal 1 - Sunucu
```bash
./ircserv 6667 mypassword
```

### Terminal 2 - Client 1
```bash
./ircclient 127.0.0.1 6667
> PASS mypassword
> NICK alice
> USER alice 0 * :Alice
> JOIN #test
> PRIVMSG #test :Hello from Alice!
```

### Terminal 3 - Client 2
```bash
./ircclient 127.0.0.1 6667
> PASS mypassword
> NICK bob
> USER bob 0 * :Bob
> JOIN #test
> PRIVMSG #test :Hi Alice!
```

---

## Kod Detayları

### `handleControlD()` Fonksiyonu

```cpp
void handleControlD()
{
    std::cout << std::endl;  // Yeni satıra geç
    std::cout.flush();
}
```

Bu fonksiyon Ctrl+D tuşuna basıldığında çağrılır ve:
1. `std::endl` ile yeni satıra geçer
2. Buffer'ı flush eder (anında gösterir)

### Raw Mode Terminal

```cpp
void enableRawMode(struct termios& orig_termios)
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);  // Echo ve canonical mode'u kapat
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
```

Bu sayede:
- Her tuş basışı anında yakalanır (Ctrl+D dahil)
- Karakterler tek tek işlenir
- Backspace çalışır

---

## Karşılaştırma

### Normal `nc`:
```bash
nc 127.0.0.1 6667
PASS secret123[ENTER]
# Ctrl+D çalışmaz veya bağlantıyı keser
```

### `ircclient`:
```bash
./ircclient 127.0.0.1 6667
> PASS sec[Ctrl+D]
> ret123[ENTER]
# ✅ Ctrl+D sadece yeni satıra geçer, bağlantı kesilmez!
```

---

## Debug

Client'ı debug mode'da çalıştırmak için kaynak kodda:

```cpp
// Ctrl+D yakalandığında
if (c == 4)
{
    std::cout << "\n[DEBUG] Ctrl+D detected!" << std::endl;
    handleControlD();
    std::cout << "> " << std::flush;
}
```

---

## Temizlik

```bash
make fclean  # Client ve server'ı sil
```

---

## Notlar

- Client raw mode'da çalışır (terminal settings değişir)
- Ctrl+C ile çıkarsan terminal otomatik eski haline döner
- Sunucu mesajları geldiğinde prompt otomatik güncellenir
- Buffer overflow koruması var (4096 byte)

---

## Sorun Giderme

**Problem:** Çıkış yaptıktan sonra terminal bozuk görünüyor  
**Çözüm:**
```bash
reset
```

**Problem:** Ctrl+D çalışmıyor  
**Çözüm:** Raw mode doğru etkinleştirilmemiş olabilir. Kodu kontrol et.

**Problem:** Karakterler ekranda görünmüyor  
**Çözüm:** `ECHO` flag'i kapalı, bu normal. Yazdıklarınız manuel echo ediliyor.

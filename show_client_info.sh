#!/bin/bash

cat << 'EOF'

╔══════════════════════════════════════════════════════════╗
║                                                          ║
║        IRC CLIENT - Ctrl+D Özelliği Eklendi! ✅         ║
║                                                          ║
╚══════════════════════════════════════════════════════════╝

📦 Derlenmiş Dosyalar:
   • ircserv     → IRC Sunucu
   • ircclient   → IRC Client (Ctrl+D desteği ile!)

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🚀 HIZLI BAŞLANGIÇ

1️⃣  Sunucuyu Başlat:
   ./ircserv 6667 mypassword

2️⃣  Client'ı Başlat:
   ./ircclient 127.0.0.1 6667

3️⃣  Komutları Yaz:
   > PASS mypassword
   > NICK myname
   > USER myuser 0 * :My Real Name

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

⌨️  CTRL+D ÖZELLİĞİ

Normal kullanım:
   > PASS mypassword[ENTER]

Ctrl+D ile:
   > PASS my[Ctrl+D]
   >                    ← Yeni satır!
   > password[ENTER]

✨ Ctrl+D basıldığında:
   ✓ Cursor bir alt satıra atılır
   ✓ Yeni prompt (>) gösterilir
   ✓ Bağlantı kesilmez
   ✓ Yazdıklarınız kaybolmaz

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🎮 TUŞ KOMUTLARI

   Ctrl+D    → Yeni satıra geç (cursor aşağı)
   Ctrl+C    → Client'tan çık
   Enter     → Komutu sunucuya gönder
   Backspace → Son karakteri sil

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

📝 ÖRNEK KULLANIM

Terminal 1:
   $ ./ircserv 6667 secret
   Server listening on port 6667...

Terminal 2:
   $ ./ircclient 127.0.0.1 6667
   Connected to 127.0.0.1:6667
   > PASS sec[Ctrl+D]
   > ret[ENTER]
   > NICK alice[ENTER]
   > USER alice 0 * :Alice[ENTER]

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🔧 TEKNİK DETAYLAR

handleControlD() Fonksiyonu:
   • Ctrl+D (ASCII 4) yakalanır
   • std::endl ile yeni satıra geçilir
   • Yeni prompt gösterilir
   • Bağlantı aktif kalır

Raw Mode Terminal:
   • Her tuş basışı anında yakalanır
   • ECHO ve ICANON kapatılır
   • Backspace çalışır
   • Ctrl+C ile çıkışta terminal eski haline döner

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

📚 DÖKÜMANTASYON

Detaylı bilgi için:
   • CLIENT_KULLANIM.md    → Kullanım rehberi
   • interactive_client.cpp → Kaynak kod

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

✅ DURUM: Ctrl+D özelliği başarıyla eklendi!

EOF

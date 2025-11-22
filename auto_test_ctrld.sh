#!/bin/bash

echo "========================================"
echo "  OTOMATIK CTRL+D TESTİ"
echo "========================================"
echo ""
echo "Ctrl+D (EOT) karakterini sunucuya gönderiyorum..."
echo ""

{
    # Ctrl+D karakterini gönder (ASCII 4)
    printf "Hello"
    printf "\x04"  # Ctrl+D (EOT)
    sleep 0.3
    printf "World\r\n"
    sleep 0.5
    printf "PASS test123\r\n"
    sleep 0.3
    printf "Test"
    printf "\x04"  # Ctrl+D
    sleep 0.3
    printf "ing\r\n"
    sleep 0.5
    printf "QUIT\r\n"
} | nc 127.0.0.1 6667

echo ""
echo "========================================"
echo "Test tamamlandı!"
echo ""
echo "✅ Eğer sunucu Ctrl+D aldığında yeni satır"
echo "   gönderdiyse, bu çalışıyor demektir!"
echo "========================================"

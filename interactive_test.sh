#!/bin/bash

echo "============================================"
echo "  İNTERAKTİF PARTIAL DATA TEST"
echo "============================================"
echo ""
echo "Sunucuya bağlanıyorum..."
echo ""
echo "Şimdi şunları yazacağım:"
echo "  1. 'PASS sec' (bekle)"
echo "  2. 'ret123' (Enter)"
echo "  3. 'NICK te' (bekle)"  
echo "  4. 'stuser' (Enter)"
echo ""
echo "Başlıyorum..."
echo "============================================"
echo ""

# Komutları interaktif gönder
{
    echo ">>> Gönderiliyor: 'PASS sec' (yarım komut, ENTER yok!)"
    printf "PASS sec"
    sleep 1
    
    echo ">>> Gönderiliyor: 'ret123' + ENTER (şimdi tamamlandı!)"
    printf "ret123\r\n"
    sleep 0.5
    
    echo ">>> SUNUCU CEVABI:"
    sleep 0.3
    
    echo ""
    echo ">>> Gönderiliyor: 'NICK te' (yarım komut)"
    printf "NICK te"
    sleep 1
    
    echo ">>> Gönderiliyor: 'stuser' + ENTER (tamamlandı!)"
    printf "stuser\r\n"
    sleep 0.5
    
    echo ""
    echo ">>> Gönderiliyor: 'USER test 0 * :Test' + ENTER"
    printf "USER test 0 * :Test\r\n"
    sleep 1
    
    echo ""
    echo ">>> Gönderiliyor: 'QU' (yarım)"
    printf "QU"
    sleep 0.5
    
    echo ">>> Gönderiliyor: 'IT' + ENTER (tamamlandı!)"
    printf "IT\r\n"
    sleep 0.5
    
} | nc 127.0.0.1 6667

echo ""
echo "============================================"
echo "✅ Test tamamlandı!"
echo "============================================"
echo ""
echo "Sunucu her parçayı birleştirip doğru işledi mi?"
echo "Yukarıdaki sunucu cevaplarına bakın!"

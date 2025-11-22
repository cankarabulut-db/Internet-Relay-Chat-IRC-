#!/bin/bash

echo "==================================="
echo "PARTIAL DATA MANUEL TEST"
echo "==================================="
echo ""
echo "Her satır ayrı ayrı gönderiliyor..."
echo ""

# Sunucuya bağlan ve komutları parçalı gönder
{
    echo "Gönderiliyor: 'PASS sec'"
    printf "PASS sec"
    sleep 0.5
    
    echo "Gönderiliyor: 'ret123' + ENTER"
    printf "ret123\r\n"
    sleep 0.5
    
    echo "Gönderiliyor: 'NICK tes'"
    printf "NICK tes"
    sleep 0.5
    
    echo "Gönderiliyor: 'tuser' + ENTER"
    printf "tuser\r\n"
    sleep 0.5
    
    echo "Gönderiliyor: 'USER test 0 * :Test User' + ENTER"
    printf "USER test 0 * :Test User\r\n"
    sleep 1
    
    echo "Gönderiliyor: 'QU'"
    printf "QU"
    sleep 0.5
    
    echo "Gönderiliyor: 'IT' + ENTER"
    printf "IT\r\n"
    sleep 0.5
    
} | nc 127.0.0.1 6667

echo ""
echo "==================================="
echo "Test tamamlandı!"
echo "==================================="

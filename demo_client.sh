#!/bin/bash

echo "========================================"
echo "  IRC CLIENT DEMO - Ctrl+D Testi"
echo "========================================"
echo ""
echo "Client başlatılıyor..."
echo ""
echo "ℹ️  Şunları deneyebilirsin:"
echo "   - Normal yazıp Enter'a bas"
echo "   - Ctrl+D'ye bas (yeni satıra geçer)"
echo "   - Backspace ile sil"
echo "   - Ctrl+C ile çık"
echo ""
echo "========================================"
echo ""
sleep 2

./ircclient 127.0.0.1 6667

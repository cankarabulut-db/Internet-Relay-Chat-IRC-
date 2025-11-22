# IRC Server - Partial Data Handling Test

## Ne Test Ediliyor?

IRC sunucunuzun **kısmi veri (partial data)** alımını doğru şekilde işleyip işlemediği.

## Neden Önemli?

Gerçek dünyada TCP paketleri şu nedenlerle parçalı gelebilir:
- Düşük bant genişliği
- Ağ gecikmesi
- TCP fragmentasyonu
- Yavaş istemciler

## Kodunuzdaki Çözüm

`Server.cpp` içindeki `handleClientData()` fonksiyonu bunu zaten doğru şekilde ele alıyor:

```cpp
// 1. Her recv() çağrısında gelen veriyi buffer'a ekle
clientBuffers[clientSocket] += buffer;

// 2. Sadece tam satır (\n) geldiğinde işle
while ((pos = buffer_ref.find('\n')) != std::string::npos)
{
    std::string line = buffer_ref.substr(0, pos);
    // ... process command ...
    buffer_ref = buffer_ref.substr(pos + 1); // Kalan kısmı sakla
}
```

## Manuel Test (Subject'teki Yöntem)

Terminal'de şunu çalıştırın:

```bash
nc -C 127.0.0.1 6667
```

Sonra şunu yazın ama her bölümden sonra **Ctrl+D** tuşuna basın:
1. Önce: `com` (sonra Ctrl+D)
2. Sonra: `man` (sonra Ctrl+D)  
3. Sonra: `d` (sonra Enter)

Sunucu bu 3 parçayı birleştirip "command" olarak işleyecek.

## Otomatik Test

```bash
./test_partial_data.sh
```

Bu script:
1. PASS komutunu 3 parçada gönderir: "PA" + "SS se" + "cret123\r\n"
2. Sunucunun doğru birleştirip işlediğini kontrol eder

## Sonuç

✅ **Kodunuz zaten doğru!** 
- Her istemci için ayrı buffer tutuyorsunuz (`clientBuffers[clientSocket]`)
- Gelen her veriyi buffer'a ekliyorsunuz
- Sadece `\n` geldiğinde komutu işliyorsunuz
- Kalan kısmı sonraki recv için saklıyorsunuz

Bu yaklaşım RFC 1459 IRC protokolü standartlarına uygun ve production-ready.

## Ek Notlar

- Buffer taşması koruması eklemek iyi olur (örn. max 512 byte per line)
- Flood koruması eklenebilir (çok hızlı komut göndermeyi engelle)
- Timeout mekanizması eklenebilir (yarım kalmış komutları temizle)

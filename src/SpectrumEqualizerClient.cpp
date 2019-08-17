#ifndef SPECTRUMEQUALIZERCLIENT_CPP
#define SPECTRUMEQUALIZERCLIENT_CPP

#include "SpectrumEqualizerClient.h"

SpectrumEqualizerClient::SpectrumEqualizerClient() {
    port = 32555;
    retry = 0;
    IPAddress remoteIP(239, 1, 1, 234);

    multicastUDP.begin(port);
    multicastUDP.joinMulticast(remoteIP);
}

void SpectrumEqualizerClient::readAudioFrequencies() {
    char frequencyPacket[14];

    int packetSize = multicastUDP.receivePacket((byte *) frequencyPacket, 14);
    if (packetSize == 14) {
        for (int i = 0; i < packetSize; i += 2) {
            int value = frequencyPacket[i] << 8 | frequencyPacket[i + 1];
            frequenciesLeftChannel[i / 2] = value;
        }
    } else {
        Serial.printlnf("Oh, snap, something went wrong: %d", packetSize);
        Serial.printlnf("SSID: %s", WiFi.SSID());
        Serial.printlnf("IP: %s", String(WiFi.localIP()).c_str());
        Serial.printlnf("Subnet: %s", String(WiFi.subnetMask()).c_str());
        Serial.printlnf("Gateway: %s", String(WiFi.gatewayIP()).c_str());
        Serial.printlnf("DNS: %s", String(WiFi.dnsServerIP()).c_str());
        Serial.printlnf("DHCP: %s", String(WiFi.dhcpServerIP()).c_str());
        Serial.printlnf("Remote Device: ", String(multicastUDP.remoteIP()).c_str(), multicastUDP.remotePort());

        retry++;
        if (retry > 2) {
            waitUntil(WiFi.ready);
            multicastUDP.begin(port);
            Particle.publish("Ran into a connection issue");
            retry = 0;
        }
    }
}

#endif
